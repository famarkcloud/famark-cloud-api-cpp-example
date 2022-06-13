#include <iostream>
#include <string>
#include <stdio.h>
#include "FamarkCloudAPI-CPP.h"
#include <json-c/json.h>
using namespace std;

int main()
{
    FamarkCloudAPI cloudAPI;

    // Taking Domain name, Username, Password from user
    string domainName, userName, password;
    cout << "Enter Domain Name: \n";
    cin >> domainName;
    cout << "Enter User Name: \n";
    cin >> userName;
    cout << "Enter Password: \n";
    cin >> password;

    string credentialData = "{\"DomainName\": \"" + domainName + "\", \"UserName\": \"" + userName + "\", \"Password\": \"" + password + "\"}";

    //Connect action call
    char* sessionData = cloudAPI.post_data("/Credential/Connect", credentialData, NULL);
    struct json_object* sessionObject = json_tokener_parse(sessionData);
    const char* sessionId = json_object_get_string(sessionObject);

    if (sessionId) {
        cout << "Session Id: \n" << sessionId;
    }
    else {
        return -1;
    }

    // Creating records by taking input from user
    char userResponse;
    cout << "\nDo you want to create a new record (y/n): \n";
    cin >> userResponse;
    while (userResponse == 'Y' || userResponse == 'y')
    {
        string displayName, systemName;
        cout << "Enter Display Name: \n";
        // Clearing new lines from input stream
        while (getchar() != '\n');
        getline(cin, displayName);
        cout << "Enter System Name: \n";
        getline(cin, systemName);
        string profileData = "{\"DisplayName\": \"" + displayName + "\", \"SystemName\": \"" + systemName + "\"}";

        //Calling CreateRecord action on System_Profile entity of System solution
        char* recordData = cloudAPI.post_data("/System_Profile/CreateRecord", profileData, sessionId);

        struct json_object* recordObject = json_tokener_parse(recordData);
        const char* recordId = json_object_get_string(recordObject);
        free(recordData);

        if (recordId) {
            cout << "Created Record Id: \n" << recordId;
            //Login to your famark cloud domain to see the newly created record under System => Profile
            cout << "\nDo you want to create more System record (y/n)? \n";
        }
        else {
            cout << "\nDo you want to try again (y/n)? \n";
        }
        cin >> userResponse;
    }

    //Calling RetrieveMultipleRecords action on the System_Profile entity
    string retrieveRecords = "{\"Columns\": \"DisplayName, SystemName\"}";
    char* records = cloudAPI.post_data("/System_Profile/RetrieveMultipleRecords", retrieveRecords, sessionId);
    struct json_object* recordObjects = json_tokener_parse(records);

    int numRecords = json_object_array_length(recordObjects);
    cout << "\nNumber of Records = " << numRecords;
    cout << "\nRecords present inside entity are: \n";
    struct json_object* tmp;
    for (int i = 0; i < numRecords; i++) {
        // Set in tmp the json_object of the recordObjects array at index i
        tmp = json_object_array_get_idx(recordObjects, i);
        cout << "\n Record no.: " << i + 1 << " " << json_object_to_json_string(tmp);
    }

    free(sessionData);
    free(records);
    return 0;
}