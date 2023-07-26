#include <iostream>
#include <string>
#include <stdio.h>
#include "FamarkCloudAPI-CPP.h"
#include <json/json.h>
using namespace std;

int main()
{
    /*IMPORTANT:: CHECK THE README FILE BEFORE RUNNING THE CODE*/
    FamarkCloudAPI cloudAPI;
    //Taking DomainName, Username, Password from user
    string domainName, userName, password;
    cout << "Enter Domain Name: ";
    cin >> domainName;
    cout << "Enter User Name: ";
    cin >> userName;
    cout << "Enter Password: ";
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

    Json::Value contactData;
    // Creating records by taking input from user
    char userResponse;
    cout << "\nDo you want to create a new record (y/n): \n";
    cin >> userResponse;
    while (userResponse == 'Y' || userResponse == 'y') 
    {
        string firstName, lastName, phone, email;
        cout << "Enter FirstName: ";
        cin >> firstName;
        cout << "Enter LastName: ";
        cin >> lastName;
        cout << "Enter Phone: ";
        cin >> phone;
        cout << "Enter Email: ";
        cin >> email;

        contactData["FirstName"] = firstName;
        contactData["LastName"] = lastName;
        contactData["Phone"] = phone;
        contactData["Email"] = email;

        //Calling CreateRecord action on Business_Contact entity of Business solution
        string recordData = cloudAPI.post_data("/Business_Contact/CreateRecord", contactData.toStyledString(), sessionId);

        if (!reader.parse(recordData, responseJson)) {
            cout << reader.getFormattedErrorMessages();
            return -2;
        }

        const string recordId = responseJson.asString();

        if (!recordId.empty()) {
            cout << "Created Record Id: \n" << recordId;
            //Login to your famark cloud domain to see the newly created record under Business > Contact
            cout << "\nDo you want to create more contact record (y/n)? \n";
        }
        else {
            cout << "\nDo you want to try again (y/n)? \n";
        }
        cin >> userResponse;
    }

    //Calling RetrieveMultipleRecords action on the Business_Contact entity
    Json::Value retrieveRequest;
    retrieveRequest["Columns"] = "FullName, Phone, Email, Business_ContactId";
    retrieveRequest["OrderBy"] = "FullName";
    const string records = cloudAPI.post_data("/Business_Contact/RetrieveMultipleRecords", retrieveRequest.toStyledString(), sessionId);

    Json::Value recordObjects;

    if (!reader.parse(records, recordObjects)) {
        cout << reader.getFormattedErrorMessages();
        return -3;
    }

    int numRecords = recordObjects.size();
    cout << "\nNumber of Records = " << numRecords;
    cout << "\nRecords present in response are:\n";

    Json::Value recordObject;

    for (Json::Value::ArrayIndex i = 0; i < numRecords; i++) {
        //Set in tmp the json_object of the recordObjects array at index i
        recordObject = recordObjects[i];
        cout << "\n Record no.: " << i + 1 << " " << recordObject.toStyledString();
    }

    //Updating records by taking input from user
    char userUpdateResponse;
    cout << "\nDo you want to update a new record (y/n): \n";
    cin >> userUpdateResponse;
    while (userUpdateResponse == 'Y' || userUpdateResponse == 'y')
    {
        string firstName, lastName, phone, email, contactId;
        cout << "Enter ContactId: ";
        cin >> contactId;
        cout << "Enter FirstName: ";
        cin >> firstName;
        cout << "Enter LastName: ";
        cin >> lastName;
        cout << "Enter Phone: ";
        cin >> phone;
        cout << "Enter Email: ";
        cin >> email;

        contactData["Business_ContactId"] = contactId;
        contactData["FirstName"] = firstName;
        contactData["LastName"] = lastName;
        contactData["Phone"] = phone;
        contactData["Email"] = email;

        //Calling UpdateRecord action on Business_Contact entity of Business solution
        string recordData = cloudAPI.post_data("/Business_Contact/UpdateRecord", contactData.toStyledString(), sessionId);

        if (!reader.parse(recordData, responseJson)) {
            cout << reader.getFormattedErrorMessages();
            return -2;
        }

        const string recordId = responseJson.asString();

        if (!recordId.empty()) {
            cout << "Updated Record Id: " << recordId;
            //Login to your famark cloud domain to see the newly created record under Business > Contact
            cout << "\nDo you want to update more contact record (y/n)? \n";
        }
        else {
            cout << "\nDo you want to try again (y/n)? \n";
        }

        cin >> userUpdateResponse;
    }

    //Delete records by taking input from user
    char userDeletingResponse;
    cout << "\nDo you want to delete a new record (y/n): \n";
    cin >> userDeletingResponse;
    while (userDeletingResponse == 'Y' || userDeletingResponse == 'y')
    {
        string contactId;
        cout << "Enter ContactId: ";
        cin >> contactId;

        contactData["Business_ContactId"] = contactId;

        //Calling DeleteRecord action on Business_Contact entity of Business solution
        string recordData = cloudAPI.post_data("/Business_Contact/DeleteRecord", contactData.toStyledString(), sessionId);

        cout << "Record Deleted";
        cout << "\nDo you want to delete a new record (y/n): \n";
        cin >> userDeletingResponse;
    }

    return 0;
}