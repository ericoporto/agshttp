// agshttp.cpp : Defines the exported functions for the DLL application.
// this plugin is supposed to be a wrapper for http.h functions
// so http (non-https) post and request can be used from AGS

#include "stdafx.h" 

#ifdef __linux__ 
#define LINUX_VERSION 1
#endif

#define HTTP_IMPLEMENTATION

#include <stdio.h>
#include "http.h" 
#include <cstring>
#define THIS_IS_THE_PLUGIN
#include "agsplugin.h"

// ags stuff
IAGSEditor *editor; // Editor interface
IAGSEngine* engine; // Engine interface
char const* agsHttpStructname = "httpRequest";

int32 dummydata = 0;

struct httpRequest {
    int ResponseSize;
    int StatusCode;
    int RequestState;
    const char * ResponseData;
    const char * ReasonPhrase;
    const char * ContentType;
    void * requestHandler;
};

httpRequest * update_httpRequest(httpRequest * aRequestStruct){
	http_t * requestHandler = (http_t*) &(aRequestStruct->requestHandler);
    aRequestStruct->ResponseSize = requestHandler->response_size;
    aRequestStruct->StatusCode = requestHandler->status_code;
    aRequestStruct->RequestState = requestHandler->status;
//    aRequestStruct->ResponseData = engine->CreateScriptString((char const*)requestHandler->response_data);
//    aRequestStruct->ReasonPhrase = engine->CreateScriptString(requestHandler->reason_phrase);
//    aRequestStruct->ContentType = engine->CreateScriptString(requestHandler->content_type);
    return aRequestStruct;
}

// -- objects --

class httpRequestObject : public IAGSScriptManagedObject {
public:
	// when a ref count reaches 0, this is called with the address
	// of the object. Return 1 to remove the object from memory, 0 to
	// leave it
	virtual int Dispose(const char *address, bool force);
	// return the type name of the object
	virtual const char *GetType();
	// serialize the object into BUFFER (which is BUFSIZE bytes)
	// return number of bytes used
	virtual int Serialize(const char *address, char *buffer, int bufsize);
};

class httpRequestObjectReader : public IAGSManagedObjectReader {
public:
	virtual void Unserialize(int key, const char *serializedData, int dataSize);
};

// -- standard methods for objects

int httpRequestObject::Dispose(const char *address, bool force)
{
	return 1;
}

const char* httpRequestObject::GetType()
{
	return agsHttpStructname;
}

int httpRequestObject::Serialize(const char *address, char *buffer, int bufsize)
{
	//fake serialization
	// put 1 byte there
	memcpy(buffer, &dummydata, sizeof(dummydata));
	return sizeof(dummydata);
}

httpRequestObject httpRO;
httpRequestObjectReader httpROR;
httpRequest httpRequestInAGS;

void httpRequestObjectReader::Unserialize(int key, const char *serializedData, int dataSize)
{
	engine->RegisterUnserializedObject(key, &httpRequestInAGS, &httpRO);
}


//------------------------------------------------------------
// Public Plugin Interfaces
//------------------------------------------------------------

httpRequest * agshttp_Get(char * url )
{
	httpRequest* pagsRequestHandler = new httpRequest;
	
    pagsRequestHandler->requestHandler = (void*) http_get(const_cast<char*>(url), NULL );
    update_httpRequest(pagsRequestHandler);
    
    engine->RegisterManagedObject(agsHttpStructname, &httpRO);
    return pagsRequestHandler;
}

httpRequest * agshttp_Post(char * url, char * data)
{
    std::size_t data_size = std::strlen(data);
	
	httpRequest* pagsRequestHandler = new httpRequest;
	
    pagsRequestHandler->requestHandler = (void*) http_post(const_cast<char*>(url), data, data_size, NULL  );
    update_httpRequest(pagsRequestHandler);
    
    engine->RegisterManagedObject(agsHttpStructname, &httpRO);
    return pagsRequestHandler;
}

int httpRequest_Process(httpRequest* agsReqHandler)
{	
    if(agsReqHandler != NULL && agsReqHandler->requestHandler != NULL){
		int temp_status = http_process((http_t*) agsReqHandler->requestHandler);
		update_httpRequest(agsReqHandler);
		return temp_status;
    }
	return -1;
}

void httpRequest_Release(httpRequest* agsReqHandler)
{
    if(agsReqHandler != NULL){
        http_release((http_t*) agsReqHandler->requestHandler);
		update_httpRequest(agsReqHandler);
        agsReqHandler = NULL;
    }
}


char * httpRequest_Get_ResponseData(httpRequest* agsReqHandler)
{
    if(agsReqHandler != NULL){
	http_t * requestHandler = (http_t*) &(agsReqHandler->requestHandler);
        return engine->CreateScriptString((char const*)requestHandler->response_data);
    }
}

char * httpRequest_Get_ReasonPhrase(httpRequest* agsReqHandler)
{
    if(agsReqHandler != NULL){
	http_t * requestHandler = (http_t*) &(agsReqHandler->requestHandler);
        return engine->CreateScriptString(requestHandler->reason_phrase);
    }
}

char * httpRequest_Get_ContentType(httpRequest* agsReqHandler)
{
    if(agsReqHandler != NULL){
	http_t * requestHandler = (http_t*) &(agsReqHandler->requestHandler);
        return engine->CreateScriptString(requestHandler->content_type);
    }
}




//------------------------------------------------------------
// Engine stuff
//------------------------------------------------------------

void AGS_EngineStartup(IAGSEngine *lpEngine)
{
    engine = lpEngine;

    engine->RegisterScriptFunction("agshttp::Get", (void*)&agshttp_Get);
    engine->RegisterScriptFunction("agshttp::Post", (void*)&agshttp_Post);
    engine->RegisterScriptFunction("httpRequest::Process", (void*)&httpRequest_Process);
    engine->RegisterScriptFunction("httpRequest::Release", (void*)&httpRequest_Release);
	
    engine->RegisterScriptFunction("httpRequest::Get_ResponseData", (void*)&httpRequest_Get_ResponseData);
    engine->RegisterScriptFunction("httpRequest::Get_ReasonPhrase", (void*)&httpRequest_Get_ReasonPhrase);
    engine->RegisterScriptFunction("httpRequest::Get_ContentType", (void*)&httpRequest_Get_ContentType);

    engine->AddManagedObjectReader(agsHttpStructname, &httpROR);
}

void AGS_EngineShutdown()
{

}

int AGS_EngineOnEvent(int event, int data)
{
    return 0;
}

int AGS_EngineDebugHook(const char *scriptName, int lineNum, int reserved)
{
    return 0;
}

void AGS_EngineInitGfx(const char *driverID, void *data)
{

}

//------------------------------------------------------------
// Editor stuff
//------------------------------------------------------------

const char* AGS_GetPluginName(void)
{
    // Return the plugin description
    return "agshttp";
}

void AGS_EditorLoadGame(char *buffer, int bufsize)
{
    // Nothing to load for this plugin
}

void AGS_EditorProperties(HWND parent)
{
    // Nothing here, but could show up a message box
}

int AGS_EditorSaveGame(char *buffer, int bufsize)
{
    // We don't want to save any persistent data
    return 0;
}

const char* scriptHeader =
"/// agshttp things\r\n"
"\r\n"
"enum http_status_t {\r\n"
"  eHttpStatusPending = 1,\r\n"
"  eHttpStatusCompleted = 0,\r\n"
"  eHttpStatusFailed = 2\r\n"
"};\r\n"
"\r\n"
"managed struct httpRequest {\r\n"
"readonly int ResponseSize;\r\n"
"readonly int StatusCode;\r\n"
"readonly int RequestState;\r\n"
"readonly void requestHandler;\r\n"
"\r\n"
"/// Poll a request handler for updates\r\n"
"import static int Process (httpRequest* agsReqHandler); // $AUTOCOMPLETESTATICONLY$\r\n"
"\r\n"
"/// Release a request handler\r\n"
"import static void Release (httpRequest* agsReqHandler); // $AUTOCOMPLETESTATICONLY$\r\n"
	
"/// Get Response Data r\r\n"
"import static String Get_ResponseData (httpRequest* agsReqHandler); \r\n"
	
"/// Get Reason Phrase\r\n"
"import static String Get_ReasonPhrase (httpRequest* agsReqHandler); \r\n"
	
"/// Get Content Type \r\n"
"import static String Get_ContentType (httpRequest* agsReqHandler); \r\n"
"\r\n"
"};\r\n"
"\r\n"
"struct agshttp {\r\n"
"\r\n"
"/// Do a Get http request and returns a handler\r\n"
"import static httpRequest* Get (String url); // $AUTOCOMPLETESTATICONLY$\r\n"
"\r\n"
"/// Do a Post http request and returns a handler\r\n"
"import static httpRequest* Post (String url,String data); // $AUTOCOMPLETESTATICONLY$\r\n"
"\r\n"
"};\r\n"
"\r\n"
  ;

int  AGS_EditorStartup(IAGSEditor* lpEditor)
{
    // User has checked the plugin to use it in their game

    // If it's an earlier version than what we need, abort.
    if (lpEditor->version < 1)
        return -1;

    editor = lpEditor;
    editor->RegisterScriptHeader(scriptHeader);

    // Return 0 to indicate success
    return 0;
}

void AGS_EditorShutdown()
{
    // User has un-checked the plugin, unregister the header!
    if (scriptHeader != nullptr) editor->UnregisterScriptHeader(scriptHeader);
}
