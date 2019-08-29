/**
 * @file error.h
 * @brief Error codes description
 *
 * @section License
 *
 * Copyright (C) 2010-2019 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Eval.
 *
 * This software is provided in source form for a short-term evaluation only. The
 * evaluation license expires 90 days after the date you first download the software.
 *
 * If you plan to use this software in a commercial product, you are required to
 * purchase a commercial license from Oryx Embedded SARL.
 *
 * After the 90-day evaluation period, you agree to either purchase a commercial
 * license or delete all copies of this software. If you wish to extend the
 * evaluation period, you must contact sales@oryx-embedded.com.
 *
 * This evaluation software is provided "as is" without warranty of any kind.
 * Technical support is available as an option during the evaluation period.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.9.4
 **/

#ifndef _ERROR_H
#define _ERROR_H

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif

#define _SYS_ERRNO_H_

#define EPERM           1               /* Not owner */
#define ENOENT          2               /* No such file or directory */
#define ESRCH           3               /* No such process */
#define EINTR           4               /* Interrupted system call */
#define EIO             5               /* I/O error */
#define ENXIO           6               /* No such device or address */
#define E2BIG           7               /* Arg list too long */
#define ENOEXEC         8               /* Exec format error */
#define EBADF           9               /* Bad file number */
#define ECHILD          10              /* No children */
#define EAGAIN          11              /* No more processes */
#define ENOMEM          12              /* Not enough space */
#define EACCES          13              /* Permission denied */
#define EFAULT          14              /* Bad address */
#define ENOTBLK         15              /* Block device required */
#define EBUSY           16              /* Device or resource busy */
#define EEXIST          17              /* File exists */
#define EXDEV           18              /* Cross-device link */
#define ENODEV          19              /* No such device */
#define ENOTDIR         20              /* Not a directory */
#define EISDIR          21              /* Is a directory */
#define EINVAL          22              /* Invalid argument */
#define ENFILE          23              /* Too many open files in system */
#define EMFILE          24              /* File descriptor value too large */
#define ENOTTY          25              /* Not a character device */
#define ETXTBSY         26              /* Text file busy */
#define EFBIG           27              /* File too large */
#define ENOSPC          28              /* No space left on device */
#define ESPIPE          29              /* Illegal seek */
#define EROFS           30              /* Read-only file system */
#define EMLINK          31              /* Too many links */
#define EPIPE           32              /* Broken pipe */
#define EDOM            33              /* Math arg out of domain of func */
#define ERANGE          34              /* Math result not representable */
#define ENOMSG          35              /* No message of desired type */
#define EIDRM           36              /* Identifier removed */
#define ECHRNG          37              /* Channel number out of range */
#define EL2NSYNC        38              /* Level 2 not synchronized */
#define EL3HLT          39              /* Level 3 halted */
#define EL3RST          40              /* Level 3 reset */
#define ELNRNG          41              /* Link number out of range */
#define EUNATCH         42              /* Protocol driver not attached */
#define ENOCSI          43              /* No CSI structure available */
#define EL2HLT          44              /* Level 2 halted */
#define EDEADLK         45              /* Deadlock */
#define ENOLCK          46              /* No lock */
#define EBADE           50              /* Invalid exchange */
#define EBADR           51              /* Invalid request descriptor */
#define EXFULL          52              /* Exchange full */
#define ENOANO          53              /* No anode */
#define EBADRQC         54              /* Invalid request code */
#define EBADSLT         55              /* Invalid slot */
#define EDEADLOCK       56              /* File locking deadlock error */
#define EBFONT          57              /* Bad font file fmt */
#define ENOSTR          60              /* Not a stream */
#define ENODATA         61              /* No data (for no delay io) */
#define ETIME           62              /* Stream ioctl timeout */
#define ENOSR           63              /* No stream resources */
#define ENONET          64              /* Machine is not on the network */
#define ENOPKG          65              /* Package not installed */
#define EREMOTE         66              /* The object is remote */
#define ENOLINK         67              /* Virtual circuit is gone */
#define EADV            68              /* Advertise error */
#define ESRMNT          69              /* Srmount error */
#define ECOMM           70              /* Communication error on send */
#define EPROTO          71              /* Protocol error */
#define EMULTIHOP       74              /* Multihop attempted */
#define ELBIN           75              /* Inode is remote (not really error) */
#define EDOTDOT         76              /* Cross mount point (not really error) */
#define EBADMSG         77              /* Bad message */
#define EFTYPE          79              /* Inappropriate file type or format */
#define ENOTUNIQ        80              /* Given log. name not unique */
#define EBADFD          81              /* f.d. invalid for this operation */
#define EREMCHG         82              /* Remote address changed */
#define ELIBACC         83              /* Can't access a needed shared lib */
#define ELIBBAD         84              /* Accessing a corrupted shared lib */
#define ELIBSCN         85              /* .lib section in a.out corrupted */
#define ELIBMAX         86              /* Attempting to link in too many libs */
#define ELIBEXEC        87              /* Attempting to exec a shared library */
#define ENOSYS          88              /* Function not implemented */
#define ENMFILE         89              /* No more files */
#define ENOTEMPTY       90              /* Directory not empty */
#define ENAMETOOLONG    91              /* File or path name too long */
#define ELOOP           92              /* Too many symbolic links */
#define EOPNOTSUPP      95              /* Operation not supported on socket */
#define EPFNOSUPPORT    96              /* Protocol family not supported */
#define ECONNRESET      104             /* Connection reset by peer */
#define ENOBUFS         105             /* No buffer space available */
#define EAFNOSUPPORT    106             /* Address family not supported by protocol family */
#define EPROTOTYPE      107             /* Protocol wrong type for socket */
#define ENOTSOCK        108             /* Socket operation on non-socket */
#define ENOPROTOOPT     109             /* Protocol not available */
#define ESHUTDOWN       110             /* Can't send after socket shutdown */
#define ECONNREFUSED    111             /* Connection refused */
#define EADDRINUSE      112             /* Address already in use */
#define ECONNABORTED    113             /* Software caused connection abort */
#define ENETUNREACH     114             /* Network is unreachable */
#define ENETDOWN        115             /* Network interface is not configured */
#define ETIMEDOUT       116             /* Connection timed out */
#define EHOSTDOWN       117             /* Host is down */
#define EHOSTUNREACH    118             /* Host is unreachable */
#define EINPROGRESS     119             /* Connection already in progress */
#define EALREADY        120             /* Socket already connected */
#define EDESTADDRREQ    121             /* Destination address required */
#define EMSGSIZE        122             /* Message too long */
#define EPROTONOSUPPORT 123             /* Unknown protocol */
#define ESOCKTNOSUPPORT 124             /* Socket type not supported */
#define EADDRNOTAVAIL   125             /* Address not available */
#define ENETRESET       126             /* Connection aborted by network */
#define EISCONN         127             /* Socket is already connected */
#define ENOTCONN        128             /* Socket is not connected */
#define ETOOMANYREFS    129
#define EPROCLIM        130
#define EUSERS          131
#define EDQUOT          132
#define ESTALE          133
#define ENOTSUP         134             /* Not supported */
#define ENOMEDIUM       135             /* No medium (in tape drive) */
#define ENOSHARE        136             /* No such host or network path */
#define ECASECLASH      137             /* Filename exists with different case */
#define EILSEQ          138             /* Illegal byte sequence */
#define EOVERFLOW       139             /* Value too large for defined data type */
#define ECANCELED       140             /* Operation canceled */
#define ENOTRECOVERABLE 141             /* State not recoverable */
#define EOWNERDEAD      142             /* Previous owner died */
#define ESTRPIPE        143             /* Streams pipe error */
/* From cygwin32.  */
#define EWOULDBLOCK     EAGAIN          /* Operation would block */
#define __ELASTERROR    2000            /* Users can add values starting here */

#define EHAVEOOB        217             /* out-of-band */
#define EIEIO           227             /* bad input/output on socket */

/**
 * @brief Error codes
 **/

typedef enum
{
   NO_ERROR = 0,                    ///<Success
   ERROR_FAILURE = 1,               ///<Generic error code

   ERROR_INVALID_PARAMETER,         ///<Invalid parameter
   ERROR_PARAMETER_OUT_OF_RANGE,    ///<Specified parameter is out of range

   ERROR_BAD_CRC,
   ERROR_BAD_BLOCK,
   ERROR_INVALID_RECIPIENT,         ///<Invalid recipient
   ERROR_INVALID_INTERFACE,         ///<Invalid interface
   ERROR_INVALID_ENDPOINT,          ///<Invalid endpoint
   ERROR_INVALID_ALT_SETTING,       ///<Alternate setting does not exist
   ERROR_UNSUPPORTED_REQUEST,       ///<Unsupported request
   ERROR_UNSUPPORTED_CONFIGURATION, ///<Unsupported configuration
   ERROR_UNSUPPORTED_FEATURE,       ///<Unsupported feature
   ERROR_ENDPOINT_BUSY,             ///<Endpoint already in use
   ERROR_USB_RESET,
   ERROR_ABORTED,

   ERROR_OUT_OF_MEMORY = 100,
   ERROR_OUT_OF_RESOURCES,
   ERROR_INVALID_REQUEST,
   ERROR_NOT_IMPLEMENTED,
   ERROR_VERSION_NOT_SUPPORTED,
   ERROR_INVALID_SYNTAX,
   ERROR_AUTHENTICATION_FAILED,
   ERROR_UNEXPECTED_RESPONSE,
   ERROR_INVALID_RESPONSE,
   ERROR_UNEXPECTED_VALUE,

   ERROR_OPEN_FAILED = 200,
   ERROR_CONNECTION_FAILED,
   ERROR_CONNECTION_REFUSED,
   ERROR_CONNECTION_CLOSING,
   ERROR_CONNECTION_RESET,
   ERROR_NOT_CONNECTED,
   ERROR_ALREADY_CLOSED,
   ERROR_ALREADY_CONNECTED,
   ERROR_INVALID_SOCKET,
   ERROR_PROTOCOL_UNREACHABLE,
   ERROR_PORT_UNREACHABLE,
   ERROR_INVALID_FRAME,
   ERROR_INVALID_HEADER,
   ERROR_WRONG_CHECKSUM,
   ERROR_WRONG_IDENTIFIER,
   ERROR_WRONG_CLIENT_ID,
   ERROR_WRONG_SERVER_ID,
   ERROR_WRONG_COOKIE,
   ERROR_NO_RESPONSE,
   ERROR_RECEIVE_QUEUE_FULL,
   ERROR_TIMEOUT,
   ERROR_WOULD_BLOCK,
   ERROR_INVALID_NAME,
   ERROR_INVALID_OPTION,
   ERROR_UNEXPECTED_STATE,
   ERROR_INVALID_COMMAND,
   ERROR_INVALID_PROTOCOL,
   ERROR_INVALID_STATUS,
   ERROR_INVALID_ADDRESS,
   ERROR_INVALID_MESSAGE,
   ERROR_INVALID_KEY,
   ERROR_INVALID_KEY_LENGTH,
   ERROR_INVALID_EPOCH,
   ERROR_INVALID_SEQUENCE_NUMBER,
   ERROR_INVALID_CHARACTER,
   ERROR_INVALID_LENGTH,
   ERROR_INVALID_PADDING,
   ERROR_INVALID_MAC,
   ERROR_INVALID_TAG,
   ERROR_INVALID_TYPE,
   ERROR_INVALID_VALUE,
   ERROR_INVALID_CLASS,
   ERROR_INVALID_VERSION,
   ERROR_INVALID_PIN_CODE,
   ERROR_WRONG_LENGTH,
   ERROR_WRONG_TYPE,
   ERROR_WRONG_ENCODING,
   ERROR_WRONG_VALUE,
   ERROR_INCONSISTENT_VALUE,
   ERROR_UNSUPPORTED_TYPE,
   ERROR_UNSUPPORTED_ALGO,
   ERROR_UNSUPPORTED_CIPHER_SUITE,
   ERROR_UNSUPPORTED_CIPHER_MODE,
   ERROR_UNSUPPORTED_CIPHER_ALGO,
   ERROR_UNSUPPORTED_HASH_ALGO,
   ERROR_UNSUPPORTED_KEY_EXCH_METHOD,
   ERROR_UNSUPPORTED_SIGNATURE_ALGO,
   ERROR_UNSUPPORTED_ELLIPTIC_CURVE,
   ERROR_INVALID_SIGNATURE_ALGO,
   ERROR_CERTIFICATE_REQUIRED,
   ERROR_MESSAGE_TOO_LONG,
   ERROR_OUT_OF_RANGE,
   ERROR_MESSAGE_DISCARDED,

   ERROR_INVALID_PACKET,
   ERROR_BUFFER_EMPTY,
   ERROR_BUFFER_OVERFLOW,
   ERROR_BUFFER_UNDERFLOW,

   ERROR_INVALID_RESOURCE,
   ERROR_INVALID_PATH,
   ERROR_NOT_FOUND,
   ERROR_ACCESS_DENIED,
   ERROR_NOT_WRITABLE,
   ERROR_AUTH_REQUIRED,

   ERROR_TRANSMITTER_BUSY,
   ERROR_NO_RUNNING,

   ERROR_INVALID_FILE = 300,
   ERROR_FILE_NOT_FOUND,
   ERROR_FILE_OPENING_FAILED,
   ERROR_FILE_READING_FAILED,
   ERROR_END_OF_FILE,
   ERROR_UNEXPECTED_END_OF_FILE,
   ERROR_UNKNOWN_FILE_FORMAT,

   ERROR_INVALID_DIRECTORY,
   ERROR_DIRECTORY_NOT_FOUND,

   ERROR_FILE_SYSTEM_NOT_SUPPORTED = 400,
   ERROR_UNKNOWN_FILE_SYSTEM,
   ERROR_INVALID_FILE_SYSTEM,
   ERROR_INVALID_BOOT_SECTOR_SIGNATURE,
   ERROR_INVALID_SECTOR_SIZE,
   ERROR_INVALID_CLUSTER_SIZE,
   ERROR_INVALID_FILE_RECORD_SIZE,
   ERROR_INVALID_INDEX_BUFFER_SIZE,
   ERROR_INVALID_VOLUME_DESCRIPTOR_SIGNATURE,
   ERROR_INVALID_VOLUME_DESCRIPTOR,
   ERROR_INVALID_FILE_RECORD,
   ERROR_INVALID_INDEX_BUFFER,
   ERROR_INVALID_DATA_RUNS,
   ERROR_WRONG_TAG_IDENTIFIER,
   ERROR_WRONG_TAG_CHECKSUM,
   ERROR_WRONG_MAGIC_NUMBER,
   ERROR_WRONG_SEQUENCE_NUMBER,
   ERROR_DESCRIPTOR_NOT_FOUND,
   ERROR_ATTRIBUTE_NOT_FOUND,
   ERROR_RESIDENT_ATTRIBUTE,
   ERROR_NOT_RESIDENT_ATTRIBUTE,
   ERROR_INVALID_SUPER_BLOCK,
   ERROR_INVALID_SUPER_BLOCK_SIGNATURE,
   ERROR_INVALID_BLOCK_SIZE,
   ERROR_UNSUPPORTED_REVISION_LEVEL,
   ERROR_INVALID_INODE_SIZE,
   ERROR_INODE_NOT_FOUND,

   ERROR_UNEXPECTED_MESSAGE = 500,

   ERROR_URL_TOO_LONG,
   ERROR_QUERY_STRING_TOO_LONG,

   ERROR_NO_ADDRESS,
   ERROR_NO_BINDING,
   ERROR_NOT_ON_LINK,
   ERROR_USE_MULTICAST,
   ERROR_NAK_RECEIVED,
   ERROR_EXCEPTION_RECEIVED,

   ERROR_NO_CARRIER,

   ERROR_INVALID_LEVEL,
   ERROR_WRONG_STATE,
   ERROR_END_OF_STREAM,
   ERROR_LINK_DOWN,
   ERROR_INVALID_OPTION_LENGTH,
   ERROR_IN_PROGRESS,

   ERROR_NO_ACK,
   ERROR_INVALID_METADATA,
   ERROR_NOT_CONFIGURED,
   ERROR_NAME_RESOLUTION_FAILED,
   ERROR_NO_ROUTE,

   ERROR_WRITE_FAILED,
   ERROR_READ_FAILED,
   ERROR_UPLOAD_FAILED,

   ERROR_INVALID_SIGNATURE,
   ERROR_INVALID_TICKET,

   ERROR_BAD_RECORD_MAC,
   ERROR_RECORD_OVERFLOW,
   ERROR_HANDSHAKE_FAILED,
   ERROR_NO_CERTIFICATE,
   ERROR_BAD_CERTIFICATE,
   ERROR_UNSUPPORTED_CERTIFICATE,
   ERROR_CERTIFICATE_EXPIRED,
   ERROR_UNKNOWN_CA,
   ERROR_DECODING_FAILED,
   ERROR_DECRYPTION_FAILED,
   ERROR_ILLEGAL_PARAMETER,
   ERROR_MISSING_EXTENSION,
   ERROR_UNSUPPORTED_EXTENSION,
   ERROR_INAPPROPRIATE_FALLBACK,
   ERROR_NO_APPLICATION_PROTOCOL,

   ERROR_MORE_DATA_REQUIRED,
   ERROR_TLS_NOT_SUPPORTED,
   ERROR_PRNG_NOT_READY,
   ERROR_SERVICE_CLOSING,
   ERROR_INVALID_TIMESTAMP,
   ERROR_NO_DNS_SERVER,

   ERROR_OBJECT_NOT_FOUND,
   ERROR_INSTANCE_NOT_FOUND,
   ERROR_ADDRESS_NOT_FOUND,

   ERROR_UNKNOWN_IDENTITY,
   ERROR_UNKNOWN_ENGINE_ID,
   ERROR_UNKNOWN_USER_NAME,
   ERROR_UNKNOWN_CONTEXT,
   ERROR_UNAVAILABLE_CONTEXT,
   ERROR_UNSUPPORTED_SECURITY_LEVEL,
   ERROR_NOT_IN_TIME_WINDOW,
   ERROR_AUTHORIZATION_FAILED,

   ERROR_INVALID_FUNCTION_CODE,
   ERROR_DEVICE_BUSY,

   ERROR_REQUEST_REJECTED,

   ERROR_INVALID_CHANNEL,
   ERROR_UNKNOWN_SERVICE,
   ERROR_UNKNOWN_REQUEST,
   ERROR_FLOW_CONTROL,

   ERROR_NO_MATCH,
   ERROR_PARTIAL_MATCH
} error_t;

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
