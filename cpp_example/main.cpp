#include <stdio.h>
#include "FamarkCloudAPI-C.h"
#include <json-c/json.h>

int main()
{
    char* credentialData = "{\"DomainName\": \"Famarker\", \"UserName\": \"TestUser1\", \"Password\": \"********\"}";
    
    //Connect action call
	char* sessionData = famark_api_post_data("/Credential/Connect", credentialData, NULL);
    
    struct json_object* sessionObject = json_tokener_parse(sessionData);
    char* sessionId = json_object_get_string(sessionObject);

    if (sessionId) {
        printf("Session Id: %s\n", sessionId);
    }
    else {
        return -1;
    }
    
    char* profileData = "{\"DisplayName\": \"Project Manager\", \"SystemName\": \"ProjectManager\"}";
    
    //Calling CreateRecord action on System_Profile entity of System solution
    char* recordData = famark_api_post_data("/System_Profile/CreateRecord", profileData, sessionId);

    struct json_object* recordObject = json_tokener_parse(recordData);
    char* recordId = json_object_get_string(recordObject);

    int retValue = 0;

    if (recordId) {
        printf("Created Record Id: %s\n", recordId);
        //Login to your famark cloud domain to see the newly created record under System => Profile
    }
    else {
        retValue = -2;
    }

    free(sessionData);
    free(recordData);
    return retValue;
}