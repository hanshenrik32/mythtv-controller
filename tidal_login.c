#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>


#define TIDAL_AUTH_URL https://api.tidal.com/oauth/authorize
#define TIDAL_TOKEN_URL https://api.tidal.com/oauth/token
 
static void getAuthorizationCode(const char *clientId, const char *clientSecret, const char *redirectUri, char *code) {
    struct curl_httppost *post = NULL;
    struct curl_slist *headers = NULL;
 
    // Create POST request
    curl_easy_setopt(curl, CURLOPT_URL, TIDAL_AUTH_URL);
 
    // Set authorization header with client ID and redirect URI
    headers = curl_slist_append(headers, "Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ=="); // Replace with your client ID as base64 encoded string
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    // Prepare POST data with grant type and redirect URI
    char postData[] = "grant_type=authorization_code&client_id=" + clientId + "&redirect_uri=" + redirectUri;
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
 
    // Execute POST request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error performing POST request: %s\n", curl_easy_strerror(res));
        return;
    }

    // Parse response JSON for authorization code
    struct curl_httppost *cur;
    curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &res);
    if (res != 200) {
        fprintf(stderr, "Error receiving response: %d\n", res);
        return;
    }

    // Parse JSON response
    char *response = malloc(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CONTENT_LENGTH_DOWNLOAD, NULL));
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CONTENT_STRING, &response);
    free(response);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

 

static void getAccessToken(const char *clientId, const char *clientSecret, const char *authCode, char *accessToken) {
    struct curl_httppost *post = NULL;
    struct curl_slist *headers = NULL;

    // Create POST request
    curl_easy_setopt(curl, CURLOPT_URL, TIDAL_TOKEN_URL);
    // Set authorization header with client ID, client secret, and grant type
    headers = curl_slist_append(headers, "Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ=="); // Replace with your client ID and client secret as base64 encoded string
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    // Prepare POST data with grant type, client ID, client secret, and authorization code
    char postData[] = "grant_type=authorization_code&client_id=" + clientId + "&client_secret=" + clientSecret + "&code=" + authCode;
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    // Execute POST request
    CURLcode res = curl_easy_perform(curl);
}

 

