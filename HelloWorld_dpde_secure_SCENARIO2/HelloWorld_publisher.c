#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "HelloWorld.h"
#include "HelloWorldSupport.h"
#include "HelloWorldApplication.h"

#define PEER_NAME       "publisher"

static bool subscriberConnected = false;
static struct timespec discovery_time;
static struct timespec start_time;

const struct ApplicationSecMaterial PEER_MATERIAL_DEFAULT =
{
    /* ca_perm_cert */ "file:security/ca/ca.pem",
    /* ca_id_cert */   "file:security/ca/ca.pem",
    /* peer_key */     "file:security/ca/certs/"PEER_NAME"_key.pem",
    /* peer_cert */    "file:security/ca/certs/"PEER_NAME".pem",
    /* xml_gov */      "file:security/xml/governance.p7s",
    /* xml_perm */     "file:security/xml/permissions_"PEER_NAME".p7s"
};


void
HelloWorldPublisher_on_publication_matched(void *listener_data,
                                           DDS_DataWriter * writer,
                                           const struct
                                           DDS_PublicationMatchedStatus *status)
{
    if (status->current_count_change > 0)
    {
        printf("Matched a subscriber\n");
        subscriberConnected = true;
       
	clock_gettime(CLOCK_MONOTONIC, &discovery_time);
        printf("Start Time: %ld seconds, %ld nanoseconds\n",start_time.tv_sec, start_time.tv_nsec);
        printf("Discovery Time: %ld seconds, %ld nanoseconds\n", discovery_time.tv_sec, discovery_time.tv_nsec);

        double elapsed_time_ms = (discovery_time.tv_sec - start_time.tv_sec) * 1000.0 +
                        (discovery_time.tv_nsec - start_time.tv_nsec) / 1000000.0;

        printf("Elapsed Time: %.2f ms\n", elapsed_time_ms);
    }
    else if (status->current_count_change < 0)
    {
        printf("Unmatched a subscriber\n");
    }
}

int
publisher_main_w_args(DDS_Long domain_id, char *udp_intf, char *peer,
                      DDS_Long sleep_time, DDS_Long count,
                      struct ApplicationSecMaterial *sec_material)
{
    DDS_Publisher *publisher;
    DDS_DataWriter *datawriter;
    HelloWorldDataWriter *hw_datawriter;
    struct DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_ReturnCode_t retcode;
    HelloWorld *sample = NULL;
    struct Application *application = NULL;
    DDS_Long i;
    struct DDS_DataWriterListener dw_listener =
        DDS_DataWriterListener_INITIALIZER;

    clock_gettime(CLOCK_MONOTONIC, &start_time);


    sample = HelloWorld_create();
    if (sample == NULL)
    {
        printf("failed HelloWorld_create\n");
        return 0;
    }

    application = Application_create(PEER_NAME, "subscriber", domain_id,
                                     udp_intf, peer, sleep_time, count,
                                     sec_material);

    if (application == NULL)
    {
        printf("failed Application create\n");
        goto done;
    }

    publisher = DDS_DomainParticipant_create_publisher(application->participant,
                                                       &DDS_PUBLISHER_QOS_DEFAULT,
                                                       NULL,
                                                       DDS_STATUS_MASK_NONE);
    if (publisher == NULL)
    {
        printf("publisher == NULL\n");
        goto done;
    }

#ifdef USE_RELIABLE_QOS
    dw_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
#else
    dw_qos.reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
#endif
    dw_qos.resource_limits.max_samples_per_instance = 32;
    dw_qos.resource_limits.max_instances = 2;
    dw_qos.resource_limits.max_samples = dw_qos.resource_limits.max_instances *
                                dw_qos.resource_limits.max_samples_per_instance;
    dw_qos.history.depth = 32;
    dw_qos.protocol.rtps_reliable_writer.heartbeat_period.sec = 0;
    dw_qos.protocol.rtps_reliable_writer.heartbeat_period.nanosec = 250000000;

    dw_listener.on_publication_matched =
        HelloWorldPublisher_on_publication_matched;
    datawriter =
        DDS_Publisher_create_datawriter(publisher, application->topic, &dw_qos,
                                        &dw_listener,
                                        DDS_PUBLICATION_MATCHED_STATUS);

    if (datawriter == NULL)
    {
        printf("datawriter == NULL\n");
        goto done;
    }

    retcode = Application_enable(application);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable application\n");
        goto done;
    }

    hw_datawriter = HelloWorldDataWriter_narrow(datawriter);

    //struct DDS_PublicationMatchedStatus *status = malloc(sizeof(struct DDS_PublicationMatchedStatus));

    while(subscriberConnected == false){
        OSAPI_Thread_sleep(sleep_time);
    }

    printf("%d\n",subscriberConnected);

    for (i = 0; (application->count > 0 && i < application->count) ||
         (application->count == 0); ++i)
    {
        /* two instances will be created, id = 0 and id = 1 */
        sample->id = (i % 2);
        sprintf(sample->msg, "Hello World! (%d)", i);
        printf("%s\n", sample->msg);

        retcode = HelloWorldDataWriter_write(hw_datawriter, sample, &DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK)
        {
            printf("Failed to write to sample\n");
        }

        OSAPI_Thread_sleep(application->sleep_time);
    }

  done:

    Application_delete(application);

    DDS_DataWriterQos_finalize(&dw_qos);

    if (sample != NULL)
    {
        HelloWorld_delete(sample);
    }

    return 0;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__))
int
main(int argc, char **argv)
{
    DDS_Long i = 0,
             domain_id = 0,
             sleep_time = 1000,
             count = 0;
    char *peer = NULL,
         *udp_intf = NULL;
    struct ApplicationSecMaterial sec_material = PEER_MATERIAL_DEFAULT;

    for (i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-domain"))
        {
            ++i;
            if (i == argc)
            {
                printf("-domain <domain_id>\n");
                return -1;
            }
            domain_id = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-udp_intf"))
        {
            ++i;
            if (i == argc)
            {
                printf("-udp_intf <interface>\n");
                return -1;
            }
            udp_intf = argv[i];
        }
        else if (!strcmp(argv[i], "-peer"))
        {
            ++i;
            if (i == argc)
            {
                printf("-peer <address>\n");
                return -1;
            }
            peer = argv[i];
        }
        else if (!strcmp(argv[i], "-sleep"))
        {
            ++i;
            if (i == argc)
            {
                printf("-sleep_time <sleep_time>\n");
                return -1;
            }
            sleep_time = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-count"))
        {
            ++i;
            if (i == argc)
            {
                printf("-count <count>\n");
                return -1;
            }
            count = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-h"))
        {
            Application_help(argv[0]);
            return 0;
        }
        else if (!strcmp(argv[i], "-ca-id"))
        {
            ++i;
            if (i == argc)
            {
                printf("-ca-id <ca.pem>\n");
                return -1;
            }
            sec_material.ca_id_cert = argv[i];
        }
        else if (!strcmp(argv[i], "-ca-perm"))
        {
            ++i;
            if (i == argc)
            {
                printf("-ca-perm <ca.pem>\n");
                return -1;
            }
            sec_material.ca_perm_cert = argv[i];
        }
        else if (!strcmp(argv[i], "-key"))
        {
            ++i;
            if (i == argc)
            {
                printf("-key <peer_key.pem>\n");
                return -1;
            }
            sec_material.peer_key = argv[i];
        }
        else if (!strcmp(argv[i], "-cert"))
        {
            ++i;
            if (i == argc)
            {
                printf("-cert <peer.pem>\n");
                return -1;
            }
            sec_material.peer_cert = argv[i];
        }
        else if (!strcmp(argv[i], "-gov"))
        {
            ++i;
            if (i == argc)
            {
                printf("-gov <governance.p7s>\n");
                return -1;
            }
            sec_material.xml_gov = argv[i];
        }
        else if (!strcmp(argv[i], "-perm"))
        {
            ++i;
            if (i == argc)
            {
                printf("-perm <permissions.p7s>\n");
                return -1;
            }
            sec_material.xml_perm = argv[i];
        }
        else
        {
            printf("unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count, &sec_material);
}
#elif defined(RTI_VXWORKS)
int
publisher_main(void)
{
    /* Explicitly configure args below */
    DDS_Long i = 0,
             domain_id = 0,
             sleep_time = 1000,
             count = 0;
    char *peer = NULL, /* set correct peer, e.g. "10.10.30.101" */
         *udp_intf = NULL;
    struct ApplicationSecMaterial sec_material = PEER_MATERIAL_DEFAULT;

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count, &sec_material);
}
#endif
