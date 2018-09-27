# agshttp 

[![Build status](https://ci.appveyor.com/api/projects/status/ilfeld73u10a3mo0?svg=true)](https://ci.appveyor.com/project/ericoporto/agshttp) | [![Build Status](https://travis-ci.org/ericoporto/agshttp.svg?branch=master)](https://travis-ci.org/ericoporto/agshttp)

***this is work in progress***

**not working yet**

ags plugin wrapper for Mattias Gustavsson tiny http.h lib, for http (non https) Post and Get requests.

## how does it work

*warning: the wants to be like below but is not like below yet!*

    httpRequest requestHandler = agshttp.Get( "http://www.mattiasgustavsson.com/http_test.txt" );
    
    while(requestHandler.Process() == eHttpStatusPending){
      wait(1);
    }
    
    Display(requestHandler.ResponseData);
    
    requestHandler.Release()
    
    
