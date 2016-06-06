#ifndef HTTP_h
#define HTTP_h

#include <Arduino.h>
#include <DTE.h>
#include <IP.h>

struct HttpStatus {
  char mode[5] = "GET";
  unsigned char status = 0;
  unsigned long finish = 0;
  unsigned long remain = 0;
};

class HTTP
{
private:
  DTE *dte;
  IP *ip;
  bool initialized = false;
  struct HttpStatus httpStatus;
  unsigned long serverResponseDataLength = 0;

  /**
   * Convert HTTP Method to index
   * @param  method HTTP Method
   * @return        index of Method
   *                0: GET
   *                1: POST
   *                2: HEAD
   */
  unsigned char methodIndex(const char method[]);

  /**
   * Command AT+HTTPINIT
   * @return  true: If command successful, false: Otherwise
   */
  bool atInitializeHttpService(void);

  /**
   * Command AT+HTTPTERM
   * @return  true: If command successful, false: Otherwise
   */
  bool atTerminateHttpService(void);

  /**
   * Command AT+HTTPPARA
   * @param  paramTag   Parameter Tag
   *                    "CID": Context Identifier Used, see IP Class
   *                    "URL": HTTP Client URL
   *                    "UA": User Agent
   *                    "PROIP": IP Address of HTTP Proxy Server
   *                    "PROPORT": Port of HTTP Proxy Server
   *                    "REDIR": If set to 1, automatic redirect, otherwis no, default is 0
   *                    "BREAK": Integer start address. Parameter for Method GET, for resuming broken transfer, start from BREAK to BREAKEND
   *                    "BREAKEND": Integer end address.
   *                    "TIMEOUT": HTTP Session Timeout value, range 30 - 1000 second, default is 120
   *                    "CONTENT": Used to set Content-Type in HTTP Header
   *                    "USERDATA": Used to set user's data in HTTP Header
   * @param  paramValue Parameter Value
   * @return            true: If command successful, false: Otherwise
   */
  bool atSetHttpParametersValue(const char paramTag[], const char paramValue[]);

  /**
   * Command AT+HTTPPARA
   * @param  paramTag   Parameter Tag
   *                    "CID": Context Identifier Used, see IP Class
   *                    "URL": HTTP Client URL
   *                    "UA": User Agent
   *                    "PROIP": IP Address of HTTP Proxy Server
   *                    "PROPORT": Port of HTTP Proxy Server
   *                    "REDIR": If set to 1, automatic redirect, otherwis no, default is 0
   *                    "BREAK": Integer start address. Parameter for Method GET, for resuming broken transfer, start from BREAK to BREAKEND
   *                    "BREAKEND": Integer end address.
   *                    "TIMEOUT": HTTP Session Timeout value, range 30 - 1000 second, default is 120
   *                    "CONTENT": Used to set Content-Type in HTTP Header
   *                    "USERDATA": Used to set user's data in HTTP Header
   * @param  paramValue Parameter Value
   * @return            true: If command successful, false: Otherwise
   */
  bool atSetHttpParametersValue(const __FlashStringHelper paramTag[], const char paramValue[]);

  /**
   * Command AT+HTTPDATA
   * @param  data    POST data
   * @param  timeout Timeout to input POST data
   * @return         true: If command successful, false: Otherwise
   */
  bool atInputHttpData(const char data[], unsigned int timeout = 2000);

  /**
   * Command AT+HTTPACTION=
   * @param  method HTTP Action Method
   *                0: GET
   *                1: POST
   *                2: HEAD
   * @return        true: If command successful, false: Otherwise
   */
  bool atHttpMethodAction(unsigned char method);

  /**
   * Command AT+HTTPREAD
   * @param  dataRecieved Buffer to store the data, buffer size should be edaquate
   * @param  startAddress Starting data output
   * @param  byteSize     Length for data output
   * @return              true: If command successful, false: Otherwise
   */
  bool atReadHttpServerResponse(char dataRecieved[], unsigned long startAddress, unsigned long byteSize);

  /**
   * Command AT+HTTPSTATUS
   * @return  true: If command successful, false: Otherwise
   */
  bool atReadHttpStatus(void);

public:
  HTTP(DTE &dte, IP &ip);

  /**
   * Get current HTTP Status
   * @return  HttpStatus Struct
   */
  struct HttpStatus getStatus(void);

  /**
   * Initialize HTTP
   * @param  timeout Set timeout in seconds, default: 30 (minimum)
   * @param  cid     Context Identifier, default: 1
   * @return         true: If initialize, false: Otherwise
   */
  bool initialize(unsigned int timeout = 30, unsigned char cid = 1);

  /**
   * Set User Agent
   * @param  userAgent User Agent string
   * @return           true: If success, false: Otherwise
   */
  bool setUserAgent(const char userAgent[]);

  /**
   * Submit HTTP Action
   * @param  method Method "GET", "POST", or "HEAD"
   * @param  url    HTTP Client URL
   * @param  data   POST data, if action is POST
   * @return        true: If success, false: Otherwise
   */
  bool action(const char method[], const char url[], const char data[] = NULL);

  /**
   * Read HTTP Server Response
   * @param  buffer       Buffer to store the data, buffer size should be edaquate
   * @param  length       Length data to be read
   * @param  startAddress Read data output from start address, default: 0 (beginning)
   * @return              true: If success, false: Otherwise
   */
  bool readDataReceived(char buffer[], unsigned long length, unsigned long startAddress = 0);

  /**
   * Terminate HTTP
   * @return  true: If success, false: Otherwise
   */
  bool terminate(void);
};

#endif