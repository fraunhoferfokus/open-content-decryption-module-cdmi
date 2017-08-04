# Open Content Decryption Module CDMi

The Open Content Decryption Module CDMi is to be used with the [Open Content Decryption Module (OCDM)](https://github.com/fraunhoferfokus/open-content-decryption-module). It is meant to run as a service connecting the webbrowser with the actual CDM. Any CDM that follows the Content Decryption Module Interface (CDMi) [1] can be used with this service. Together, both components enable DRM interoperability to be used with HTML5 based browser environments [2] according to W3C EME [3] specification.

This repository provides a bare bones implementation to demonstrate how a CDM can be adapted to be used with OCDM through the CDMi interface. The implementation provides the complete flow between webbrowser and CDM.

OCDM is built with a modular concept in mind to be easily extendible and flexible. For more information see the the OCDM repository.

Fraunhofer FOKUS has developed the Open Content Decryption Module (OCDM) according to W3C EME specification to be used with HTML5 based browser environments. The implementation enables DRM interoperability. We would be happy to see a broad adoption of our implementation and encourage contributions. A first e2e implementation has been done testing with a Microsoft PlayReady CDMi implementation.

## Introduction / Purpose / Why this?

* Interoperable HTML5 based protected video delivery
* DRM interoperability
  * CENC, DASH
* Plugin based integration (Pepper Plugin API)
* CDMi allows open source browsers to support DRM without licensing it
* e2e tested with Microsoft PlayReady DRM system

## References

* [1] Content Decryption Module Interface Specification http://download.microsoft.com/download/E/A/4/EA470677-6C3C-4AFE-8A86-A196ADFD0F78/Content%20Decryption%20Module%20Interface%20Specification.pdf
* [2] Fraunhofer FOKUS FAMIUM DRM http://www.fokus.fraunhofer.de/en/fame/Solutions/DRM/index.html
* [3] W3C Encrypted Media Extensions https://dvcs.w3.org/hg/html-media/raw-file/default/encrypted-media/encrypted-media.html

## Supported Platforms

The used development platform is Ubuntu Linux 14.04 64-bit, should work fine on most other Linux' as well.

## How to build

* clone this repository
* compile with GNU make
 * ```$ make```

## How to run

*  ```$ ./service```

## Known Issues / Comments

This is a preliminary release. Please file any issues or comments.

* TODO: add session-load support
* TODO: use named pipes for IPC
* Code needs more review (e.g. memory allocation, appropriate data types).

Please see the OCDM repository for further information [github.com/fraunhoferfokus/open-content-decryption-module](https://github.com/fraunhoferfokus/open-content-decryption-module).
