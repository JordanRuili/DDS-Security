# DDS-Security

WORKPLAN RESULTS
------------------
ATTACK SCENARIOS

The domain and topic rules could be changed in the governance.xml file (path : .../example/unix/C/HelloWorld_dpde_secure/security/xml)
The different rules can be found in the Hands-On 2 in the documentation : https://community.rti.com/static/documentation/connext-dds/6.0.1/doc/manuals/connext_dds/dds_security/html_files/RTI_SecurityPlugins_GettingStarted/cpp98/hands_on_2.html
Before getting started, in .../HelloWorld_dpde_secure/HelloWorldApplication I uncommented the function OSAPI_Log_set_verbosity(OSAPI_LOG_VERBOSITY_WARNING), enabling more error messages to appear in the terminal.

I tried different kind of attacks by modifying the governance.xml file in 2 different systems and making a participant in each of them discover the other.
Here are different observations I made :

DOMAIN RULES :

- allow_unauthenticated_participants can either be set on true or false, as long as rtps_protection_kind is set on NONE, a domain participant will be able to discover and communicate with another participant. However, the participant will now accept unprotected RTPS messages, thus disabling a cryptography security. 

- enable_join_access_control is a parameter that checks remote participants when they are discovered. If it is enabled, it will perform Authentification control on discovered participants and printing the "ERROR: ModuleID=7 Errcode=1492 X=0 E=1 T=1" (DDSC_LOG_TRUST_PARTICIPANT_AUTHENTICATION_FAILED_EC error) whenever it will fail to authentificate a participant. 

- all the parameters ending with _protection_kind are related to cryptography, they obviously have to be the same for every domain participants trying to communicate. I have tried all of the different values for each of them (NONE, SIGN, ENCRYPT,  SIGN_WITH_ORIGIN_AUTHENTICATION, ENCRYPT_WITH_ORIGIN_AUTHENTICATION) and here is what I observed :  
    • discovery_protection_kind has to be set on ENCRYPT otherwise the same error as before will be printed (ModuleID=7 Errcode=1492, failing the       Authentification)
    • liveliness_protection_kind has to be set on NONE or the error will appear
    • rtps_protection_kind can either be set on SIGN or NONE, but as seen before it has to be on NONE to allow unauthenticated participants
    • both of the _WITH_ORIGIN_AUTHENTICATION values don't work and fail to create application, all the errors are in the screenshot errorwithauth.png. According to the documentation, it seems that the participants are unable to use that kind of encryption as they can't generate the individual sender-receiver pair keys.

TOPIC RULES :

- enable_discovery_protection has to be set on true to be able to be announced on secure discovery endpoints, and will thus only be matchable by authenticated participant. 

- enable_write_access_control and enable_read_access_control can be set on either true of false, but the two discovery endpoints must be set on the same in order to allow the discovery of participants.

- if the domain rule allow_unauthenticated_participants is set on true, and rtps_protection_kind on NONE, it is possible to set all the topic rules on false/NONE to allow discovery and communication with unsecure participants. This setup can be found in the governance.xml file present in the Github repository. Thus, it allows a domain participant to discover and communicate with both secure and unsecure participants, but will not perform authentification check, sign RTPS messages nor include any security in topic discovery endpoints. An example of unsecure domain participant can be found in the provided HelloWorld_dpde repository.

------------------

DISCOVERY OVERHEAD

Scenario 1 :

The result repositories can be found on the github as HelloWorld_dpde_SCENARIO1 (security plugin disabled) and HelloWorld_dpde_secure_SCENARIO1 (security plugin enabled).
Finding out the time overhead with and without security enabled involved modifying the HelloWorld_publisher file in both cases. The file was changed as to start sending messages only when a subscriber is matched. Doing so, if we start by launching a subscriber, the overhead can be found by launching the publisher and calculating the time between the moment the publisher was launched and the moment a subscriber was matched. To do so, the POSIX function clock_gettime was used with CLOCK_MONOTONIC to get an accurate measurement of time. In my case, I had to change the Makefile and add the 
-D_POSIX_C_SOURCE=199309L cflag in order to run the clock_gettime function (included in time.h library). Each process is bound to a different CPU core using the command taskset -c X /objs/x64Linux4gcc9.4.0/HelloWorld_publisher (or subscriber) where X ranges from 0 to 3 (4 CPU cores).

Here are different results observations I made in localhost:
- The average discovery time for a single regular subscriber is 3.80 ms
- The average discovery time for a single secure subscriber is 25 ms
- When launching 2 subscribers, different behaviours can be observed depending on which core they are bound : 
        • if they are bound to the same core, a subscriber will be discovered way before another (regular: 4ms and 4.30ms, secure: 30ms and 80ms) 
        • if they are bound to a different CPU, the subscribers will be discovered almost simultaneously (regular: 4.30ms, secure: 80ms) 

When in the same core, the different subscribers processes are queued one after the other while when in different cores, the computer system gives ressources to the different cores in reciprocation, which results in them ending almost at the same time.
We could comment that when in the same core, the first subscriber discovering time is slightly longer than in the single subscriber case, which could result of the fact that the creation time of the processes in the processes table is longer.

Scenario 2 :

For this second scenario, the secure configuration can be found in the HelloWorld_dpde_secure_SCENARIO2, where a secure publisher and subscriber will be launched. The unauthorized subscriber can be launched HelloWorld_dpde_SCENARIO1. This scenario has the same preliminaries as the first scenario. Launching the unauthorized subscriber will print error in the secure domain participants terminal which results of the failed authentification of the regular subscriber.

Here are some observations :
- Launching the subscribers on the same core can result in two different possible results :
    • they are discovered by the publisher at almost the same time (less than 1ms of difference) in around 27ms, this case occurs most of the time
    • they are discovered separately, with the first one being discovered in around 18ms and the other at 27ms
It seems that the system scheduler gives ressources reciprocately to both of the subscribers most of time in order to have them finish at the same time.
- if more than 2 subscribers are launched on the same core, whether they are regular or secure subscribers, they are all discovered in almost the same time, with times ranging from 26 to 30ms. In that case, the scheduler seems to always alternate the ressources share between the processes.

- When launched on different CPU cores, the result will always be the same, with one the subscribers being discovered in around 18ms and the other one in 28ms.
- If more than 2 subscribers are launched, any regular subscriber will take around 18ms to be discovered and any secure subscriber will take around 28ms.
However, as the computer I am using with 4 CPU cores, I could only launch 3 subscribers apart from the publisher. To launch additional subscribers I used at least twice a same core, which resulted in the first observation made.


Script results :

The bash scripts scenario1.sh and scenario2.sh can be launched from the common repository root to test the different outputs of the workplan (participants are launched from different cores).

_____________________________
Multiple attacks:

I provided a bash script called attacks.sh which launches 10 subscribers (with security plugin enabled randomly). This script simulates an attack scenario where multiple subscribers try to attack a domain. I could not create the subscribers tabs in the same window as the publisher but it should still work. With multiple subscribers, the following errors are printed in the terminals : ERROR: ModuleID=2 Errcode=12 X=0 E=1 T=1, ERROR: ModuleID=7 Errcode=109 X=0 E=1 T=1, ERROR: ModuleID=11 Errcode=36 X=0 E=1 T=1, which means that the database's table record is full and that the participant failed to assert and discover completely a remote participant. Indeed, among the 10 subscribers launched, not all of them could be discovered by the publisher (and the other subscribers) and the number discovered varies.









