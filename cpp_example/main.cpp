#include <iostream>
#include <string>
#include <stdio.h>
#include "FamarkCloudAPI-CPP.h"
#include <json/json.h>
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

    Json::Value credData;
    credData["DomainName"] = domainName;
    credData["UserName"] = userName;
    credData["Password"] = password;

    //Connect action call
    const string sessionData = cloudAPI.post_data("/Credential/Connect", credData.toStyledString(), "");

    Json::Reader reader;
    Json::Value responseJson;

    // Parse JSON and print errors if needed
    if (!reader.parse(sessionData, responseJson)) {
        cout << reader.getFormattedErrorMessages();
        return -1;
    }

    const string sessionId = responseJson.asString();

    if (!sessionId.empty()) {
        cout << "Session Id: \n" << sessionId;
    }
    else {
        return -1;
    }

    Json::Value profileData;
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

        profileData["DisplayName"] = displayName;
        profileData["SystemName"] = systemName;

        //Calling CreateRecord action on System_Profile entity of System solution
        string recordData = cloudAPI.post_data("/System_Profile/CreateRecord", profileData.toStyledString(), sessionId);

        if (!reader.parse(recordData, responseJson)) {
            cout << reader.getFormattedErrorMessages();
            return -2;
        }

        const string recordId = responseJson.asString();

        if (!recordId.empty()) {
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
    Json::Value retrieveRequest;
    retrieveRequest["Columns"] = "DisplayName, SystemName";
    retrieveRequest["OrderBy"] = "DisplayName";
    const string records = cloudAPI.post_data("/System_Profile/RetrieveMultipleRecords", retrieveRequest.toStyledString(), sessionId);

    Json::Value recordObjects;

    if (!reader.parse(records, recordObjects)) {
        cout << reader.getFormattedErrorMessages();
        return -3;
    }

    int numRecords = recordObjects.size();
    cout << "\nNumber of Records = " << numRecords;
    cout << "\nRecords present inside entity are: \n";

    Json::Value recordObject;

    for (Json::Value::ArrayIndex i = 0; i < numRecords; i++) {
        // Set in tmp the json_object of the recordObjects array at index i
        recordObject = recordObjects[i];
        cout << "\n Record no.: " << i + 1 << " " << recordObject.toStyledString();
    }

    return 0;
}