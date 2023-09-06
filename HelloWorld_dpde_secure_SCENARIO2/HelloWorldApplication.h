#ifndef Application_h
#define Application_h

#include "rti_me_c.h"

/*---------------------------------------------------------------------------*/
/* Application wrapper                                                       */
/*---------------------------------------------------------------------------*/

/* Define USE_SECURITY to register the RTI Security Plugin factories
 * and enable them in the Application's DomainParticipant
 */
#define USE_SECURITY

/* Define USE_RELIABLE_QOS to use reliable
 * reliability on the DataReader; otherwise, use
 * best-effort reliability by default.
 */

#define USE_RELIABLE_QOS

/* Define USE_SAMPLE_FILTER to filter samples
 * using call-backs on the DataReader's listener*/

/*#define USE_SAMPLE_FILTER*/

/* Define FILTER_ON_DESERIALIZE to enable
 * filtering on call-back on_before_sample_deserialize;
 * otherwise use call-back on_before_sample_commit
 * by default. */

/*#define FILTER_ON_DESERIALIZE*/

/* Filenames of required cryptographic material and configuration files */
struct ApplicationSecMaterial
{
    char *ca_id_cert;
    char *ca_perm_cert;
    char *peer_key;
    char *peer_cert;
    char *xml_gov;
    char *xml_perm;
};

#define ApplicationSecMaterial_INITIALIZER \
{\
    NULL, /* ca_id_cert */\
    NULL, /* ca_perm_cert */\
    NULL, /* peer_key */ \
    NULL, /* peer_cert */ \
    NULL, /* xml_gov */ \
    NULL  /* xml_perm */ \
}

#define ApplicationSecMaterial_is_valid(mat_) \
(\
    (mat_) != NULL && \
    (mat_)->ca_id_cert != NULL && \
    (mat_)->ca_perm_cert != NULL && \
    (mat_)->peer_key != NULL && \
    (mat_)->peer_cert != NULL && \
    (mat_)->xml_gov != NULL && \
    (mat_)->xml_perm != NULL \
)

#define ApplicationSecMaterial_print_var(k_,v_) \
    printf("# %-18s : %s\n",(k_), (v_))

#define ApplicationSecMaterial_print(mat_) \
{\
    ApplicationSecMaterial_print_var("Identity CA,", (mat_)->ca_id_cert);   \
    ApplicationSecMaterial_print_var("Permissions CA,", (mat_)->ca_perm_cert);   \
    ApplicationSecMaterial_print_var("PEER certificate",(mat_)->peer_cert); \
    ApplicationSecMaterial_print_var("PEER key",        (mat_)->peer_key);  \
    ApplicationSecMaterial_print_var("XML governance",  (mat_)->xml_gov);   \
    ApplicationSecMaterial_print_var("XML permissions", (mat_)->xml_perm);  \
}

extern const struct ApplicationSecMaterial PEER_MATERIAL_DEFAULT;

struct Application {
    DDS_DomainParticipant *participant;
    char topic_name[255];
    char type_name[255];
    DDS_Long sleep_time;
    DDS_Long count;
    DDS_Topic *topic;
    struct ApplicationSecMaterial *sec_material;
};

#define Application_INITIALIZER \
{\
    NULL, /* participant */            \
    "\0", /* topic_name */             \
    "\0", /* type_name */              \
    0,    /* sleep_time */             \
    0,    /* count */                  \
    NULL, /* topic */                  \
    NULL  /* sec_material */ \
}


#ifdef USE_SECURITY
DDS_Boolean
Application_configure_security(
        struct Application *application,
        RT_Registry_T *registry,
        struct DDS_DomainParticipantQos *dp_qos,
        struct ApplicationSecMaterial *sec_material);

DDS_Boolean
Application_finalize_security(
        struct Application *application,
        RT_Registry_T *registry);
#endif /* USE_SECURITY */

extern void
Application_help(char *appname);

extern struct Application*
Application_create(const char *local_participant_name,
                   const char *remote_participant_name,
                   DDS_Long domain_id, char *udp_intf, char *peer,
                   DDS_Long sleep_time, DDS_Long count,
                   struct ApplicationSecMaterial *sec_material);

extern DDS_ReturnCode_t
Application_enable(struct Application *application);

extern void
Application_delete(struct Application *application);

#endif
