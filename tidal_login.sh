# client id + secret

B64CREDS=$(echo -n "Nq5WQmVhv2L7QWQO:vxOmFp39rweIVD2rb20qmpETsoAECwhGUdnPIPSXq4g=" | base64)
curl -X POST \
  -H "Authorization: Basic $B64CREDS" \
  -d "grant_type=client_credentials" \
  https://auth.tidal.com/v1/oauth2/token

#curl -X 'GET' \
#  -H 'accept: application/vnd.tidal.v1+json' \
#  -H 'Authorization: Bearer {ACCESS_TOKEN}' \
#  -H 'Content-Type: application/vnd.tidal.v1+json' \
#  'https://openapi.tidal.com/albums/59727856?countryCode=US'


# get playlist curl -X GET https://tidalapi.netlify.app/playlist -H "Authorization: Bearer [YOUR_ACCESS_TOKEN]"

