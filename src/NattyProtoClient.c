/*
 *  Author : WangBoJing , email : 1989wangbojing@gmail.com
 * 
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Author. (C) 2016
 * 
 *
 
****       *****
  ***        *
  ***        *                         *               *
  * **       *                         *               *
  * **       *                         *               *
  *  **      *                        **              **
  *  **      *                       ***             ***
  *   **     *       ******       ***********     ***********    *****    *****
  *   **     *     **     **          **              **           **      **
  *    **    *    **       **         **              **           **      *
  *    **    *    **       **         **              **            *      *
  *     **   *    **       **         **              **            **     *
  *     **   *            ***         **              **             *    *
  *      **  *       ***** **         **              **             **   *
  *      **  *     ***     **         **              **             **   *
  *       ** *    **       **         **              **              *  *
  *       ** *   **        **         **              **              ** *
  *        ***   **        **         **              **               * *
  *        ***   **        **         **     *        **     *         **
  *         **   **        **  *      **     *        **     *         **
  *         **    **     ****  *       **   *          **   *          *
*****        *     ******   ***         ****            ****           *
                                                                       *
                                                                      *
                                                                  *****
                                                                  ****


 *
 */

#include <pthread.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#include "NattyProtoClient.h"
#include "NattyTimer.h"
#include "NattyUtils.h"
#include "NattyNetwork.h"
#include "NattyResult.h"
#include "NattyVector.h"


#define NTY_RECONNECTION_TIMER_TICK		3

/* ** **** ******** **************** Global Variable **************** ******** **** ** */
static NWTimer *nHeartBeatTimer = NULL;
static NWTimer *nReconnectTimer = NULL;
static NWTimer *nBigBufferSendTimer = NULL;
static NWTimer *nBigBufferRecvTimer = NULL;
static NWTimer *nRunTimer = NULL;



static int ntyHeartBeatCb (NITIMER_ID id, void *user_data, int len);

static void ntySetupRecvProcThread(void *self);
static int ntySendLogin(void *self);
static int ntySendLogout(void *self);
static void ntySendTimeout(int len);
static void* ntyRecvProc(void *arg);

void *ntyProtoInstance(void);
void ntyProtoRelease(void);


static int ntySendBigBuffer(void *self, U8 *u8Buffer, int length, C_DEVID gId);
static int ntyReconnectCb(NITIMER_ID id, void *user_data, int len);

static void *ntyProtoGetInstance(void);


#if 1 //

typedef void* (*RECV_CALLBACK)(void *arg);

typedef enum {
	STATUS_NETWORK_LOGIN,
	STATUS_NETWORK_PROXYDATA,
	STATUS_NETWORK_LOGOUT,
} StatusNetwork;

#if 1 //local

static char *sdk_version = "NattyAndroid V5.4 betaA";

static C_DEVID gSelfId = 0;
RECV_CALLBACK onRecvCallback = NULL;
PROXY_CALLBACK onProxyCallback = NULL;
PROXY_CALLBACK onProxyFailed = NULL; //send data failed
PROXY_CALLBACK onProxySuccess = NULL; //send data success
PROXY_CALLBACK onProxyDisconnect = NULL;
PROXY_CALLBACK onProxyReconnect = NULL;
PROXY_CALLBACK onBindResult = NULL;
PROXY_CALLBACK onUnBindResult = NULL;
NTY_PACKET_CALLBACK onPacketRecv = NULL;
PROXY_CALLBACK onPacketSuccess = NULL;

NTY_PARAM_CALLBACK onLoginAckResult = NULL; //RECV LOGIN_ACK
NTY_STATUS_CALLBACK onHeartBeatAckResult = NULL; //RECV HEARTBEAT_ACK
NTY_STATUS_CALLBACK onLogoutAckResult = NULL; //RECV LOGOUT_ACK
NTY_PARAM_CALLBACK onTimeAckResult = NULL; //RECV TIME_ACK
NTY_PARAM_CALLBACK onICCIDAckResult = NULL; //RECV ICCID_ACK
NTY_RETURN_CALLBACK onCommonReqResult = NULL; //RECV COMMON_REQ

NTY_STATUS_CALLBACK onVoiceDataAckResult = NULL; //RECV VOICE_DATA_ACK
NTY_PARAM_CALLBACK onOfflineMsgAckResult = NULL; //RECV OFFLINE_MSG_ACK
NTY_PARAM_CALLBACK onLocationPushResult = NULL; //RECV LOCATION_PUSH
NTY_PARAM_CALLBACK onWeatherPushResult = NULL; //RECV WEATHER_PUSH
NTY_RETURN_CALLBACK onDataRoute = NULL; //RECV DATA_RESULT
NTY_PARAM_CALLBACK onDataResult = NULL; //RECV DATA_RESULT
NTY_RETURN_CALLBACK onVoiceBroadCastResult = NULL; //RECV VOICE_BROADCAST
NTY_RETURN_CALLBACK onLocationBroadCastResult = NULL; //RECV LOCATION_BROADCAST
NTY_RETURN_CALLBACK onCommonBroadCastResult = NULL; //RECV COMMON_BROADCAST
NTY_RETURN_CALLBACK onBindConfirmResult = NULL;
NTY_PARAM_CALLBACK onUserDataAck = NULL;

#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
U8 tokens[NORMAL_BUFFER_SIZE];
U16 tokenLen;
U8 publishStatus = 0;
#endif


U8 u8ConnectFlag = 0;


#endif


typedef struct _NATTYPROTOCOL {
	const void *_;
	C_DEVID selfId; //App Or Device Id
	C_DEVID fromId; //store ack devid
#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
	U8 tokens[NORMAL_BUFFER_SIZE];
	U8 tokenLen;
	U8 publishStatus;
#endif
	void *friends;
	U8 recvBuffer[RECV_BUFFER_SIZE];
	U16 recvLen;
	RECV_CALLBACK onRecvCallback; //recv
	PROXY_CALLBACK onProxyCallback; //just for java
	PROXY_CALLBACK onProxyFailed; //send data failed
	PROXY_CALLBACK onProxySuccess; //send data success
	PROXY_CALLBACK onProxyDisconnect;
	PROXY_CALLBACK onProxyReconnect;
	PROXY_CALLBACK onBindResult;
	PROXY_CALLBACK onUnBindResult;
	NTY_PACKET_CALLBACK onPacketRecv;
	PROXY_CALLBACK onPacketSuccess;
	
#if 1 //Natty Protocol v3.2

	NTY_PARAM_CALLBACK onLoginAckResult; //RECV LOGIN_ACK
	NTY_STATUS_CALLBACK onHeartBeatAckResult; //RECV HEARTBEAT_ACK
	NTY_STATUS_CALLBACK onLogoutAckResult; //RECV LOGOUT_ACK
	NTY_PARAM_CALLBACK onTimeAckResult; //RECV TIME_ACK
	NTY_PARAM_CALLBACK onICCIDAckResult; //RECV ICCID_ACK
	NTY_RETURN_CALLBACK onCommonReqResult; //RECV COMMON_REQ
#if 0 //discard
	NTY_PARAM_CALLBACK onCommonAckResult; //RECV COMMON_ACK
#endif
	NTY_STATUS_CALLBACK onVoiceDataAckResult; //RECV VOICE_DATA_ACK
	NTY_PARAM_CALLBACK onOfflineMsgAckResult; //RECV OFFLINE_MSG_ACK
	NTY_PARAM_CALLBACK onLocationPushResult; //RECV LOCATION_PUSH
	NTY_PARAM_CALLBACK onWeatherPushResult; //RECV WEATHER_PUSH
	NTY_RETURN_CALLBACK onDataRoute; //RECV DATA_RESULT
	NTY_PARAM_CALLBACK onDataResult; //RECV DATA_RESULT
	NTY_RETURN_CALLBACK onVoiceBroadCastResult; //RECV VOICE_BROADCAST
	NTY_RETURN_CALLBACK onLocationBroadCastResult; //RECV LOCATION_BROADCAST
	NTY_RETURN_CALLBACK onCommonBroadCastResult; //RECV COMMON_BROADCAST
	NTY_RETURN_CALLBACK onBindConfirmResult; //RECV COMMON_BROADCAST
	NTY_PARAM_CALLBACK onUserDataAck;
#endif
	pthread_t recvThreadId;
	U8 u8RecvExitFlag;
	U8 u8ConnectFlag;

} NattyProto;

typedef struct _NATTYPROTO_OPERA {
	size_t size;
	void* (*ctor)(void *_self, va_list *params);
	void* (*dtor)(void *_self);
	int (*login)(void *_self); //argument is optional
	int (*logout)(void *_self); //argument is optional
#if 0
	void (*proxyReq)(void *_self, C_DEVID toId, U8 *buffer, int length);
	void (*proxyAck)(void *_self, C_DEVID retId, U32 ack);
	void (*fenceReq)(void *_self, C_DEVID toId, U8 *buffer, int length);
	void (*fenceAck)(void *_self, C_DEVID friId, U32 ack);
#else
	int (*voiceReq)(void *_self, U32 msgId, U8 *json, U16 length);
	int (*voiceAck)(void *_self, U32 msgId, U8 *json, U16 length);
	int (*voiceDataReq)(void *_self, C_DEVID gId, U8 *data, int length);
	int (*commonReq)(void *_self, C_DEVID gId, U8 *json, U16 length);
	int (*commonAck)(void *_self, U32 msgId, U8 *json, U16 length);
	int (*offlineMsgReq)(void *_self);
	int (*dataRoute)(void *_self, C_DEVID toId, U8 *json, U16 length);
#endif
	int (*bind)(void *_self, C_DEVID did, U8 *json, U16 length);
	int (*unbind)(void *_self, C_DEVID did);
	int (*comfirmReq)(void *_self, C_DEVID proposerId, C_DEVID devId, U32 msgId, U8 *json, U16 length);
	int (*dataReq)(void *_self, U8 *json, U16 length);
	int (*perform)(void *_self, U8 *json, U16 length);
	int (*msgPush)(void *_self, C_DEVID toId, U8 *json, U16 length);
} NattyProtoOpera;

typedef NattyProtoOpera NattyProtoHandle;

#endif


void* ntyProtoClientCtor(void *_self, va_list *params) {
	NattyProto *proto = _self;

	proto->onRecvCallback = ntyRecvProc;
	proto->selfId = gSelfId;
	proto->recvLen = 0;
	memset(proto->recvBuffer, 0, RECV_BUFFER_SIZE);
	//proto->friends = ntyVectorCreator();

	proto->onProxyCallback = onProxyCallback; //just for java
	proto->onProxyFailed = onProxyFailed; //send data failed
	proto->onProxySuccess = onProxySuccess; //send data success
	proto->onProxyDisconnect = onProxyDisconnect;
	proto->onProxyReconnect = onProxyReconnect;

	proto->onBindResult = onBindResult;
	proto->onUnBindResult = onUnBindResult;
	proto->onPacketRecv = onPacketRecv;
	proto->onPacketSuccess = onPacketSuccess;
	
	proto->onLoginAckResult = onLoginAckResult; //RECV LOGIN_ACK
	proto->onHeartBeatAckResult = onHeartBeatAckResult; //RECV HEARTBEAT_ACK
	proto->onLogoutAckResult = onLogoutAckResult; //RECV LOGOUT_ACK

	proto->onTimeAckResult = onTimeAckResult; //RECV TIME_ACK
	proto->onICCIDAckResult = onICCIDAckResult; //RECV ICCID_ACK
	proto->onCommonReqResult = onCommonReqResult; //RECV COMMON_REQ

	proto->onVoiceDataAckResult = onVoiceDataAckResult; //RECV VOICE_DATA_ACK
	proto->onOfflineMsgAckResult = onOfflineMsgAckResult; //RECV OFFLINE_MSG_ACK
	proto->onLocationPushResult = onLocationPushResult; //RECV LOCATION_PUSH

	proto->onWeatherPushResult = onWeatherPushResult; //RECV WEATHER_PUSH
	proto->onDataRoute = onDataRoute; //RECV DATA_RESULT
	proto->onDataResult = onDataResult; //RECV DATA_RESULT

	proto->onVoiceBroadCastResult = onVoiceBroadCastResult; //RECV VOICE_BROADCAST
	proto->onLocationBroadCastResult = onLocationBroadCastResult; //RECV LOCATION_BROADCAST
	proto->onCommonBroadCastResult = onCommonBroadCastResult; //RECV COMMON_BROADCAST
	proto->onBindConfirmResult = onBindConfirmResult;

	proto->onUserDataAck = onUserDataAck;

#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
	memcpy(proto->tokens, tokens, tokenLen);
	proto->tokenLen = tokenLen;
	proto->publishStatus = publishStatus;
#endif	

	ntyGenCrcTable();
	//Setup Socket Connection
	Network *network = ntyNetworkInstance();
	if (network->sockfd < 0) { //Connect failed
		proto->u8ConnectFlag = 0;
	} else {
		proto->u8ConnectFlag = 1;
	}
	
	//Create Timer
	void *nTimerList = ntyTimerInstance();
	nHeartBeatTimer = ntyTimerAdd(nTimerList, HEARTBEAT_TIME_TICK, ntyHeartBeatCb, NULL, 0);

	return proto;
}


void* ntyProtoClientDtor(void *_self) {
	NattyProto *proto = _self;

	proto->onRecvCallback = NULL;
	proto->recvLen = 0;

	//Cancel Timer
	void *nTimerList = ntyTimerInstance();
	ntyTimerDel(nTimerList, nHeartBeatTimer);

	//Release Socket Connection
	ntyNetworkRelease();

	
	//ntyVectorDestory(proto->friends);
	proto->u8ConnectFlag = 0;
	proto->u8RecvExitFlag = 0;

#if 1 //should send logout packet to server
#endif

	return proto;
}

/*
 * heartbeat Packet
 * VERSION					1			BYTE
 * MESSAGE TYPE				1			BYTE (req, ack)
 * TYPE					1			BYTE 
 * DEVID					8			BYTE
 * ACKNUM					4			BYTE (Network Module Set Value)
 * CRC 					4			BYTE (Network Module Set Value)
 * 
 * send to server addr
 */
static int ntyHeartBeatCb (NITIMER_ID id, void *user_data, int len) {
	NattyProto *proto = ntyProtoGetInstance();
	ClientSocket *nSocket = ntyNetworkInstance();
	int length, n;
	U8 buffer[NTY_HEARTBEAT_ACK_LENGTH] = {0};	

	bzero(buffer, NTY_HEARTBEAT_ACK_LENGTH);
	if (proto == NULL) return NTY_RESULT_ERROR;
	
	if (proto->selfId == 0) {//set devid
		ntylog("[%s:%s:%d] selfId == 0\n", __FILE__, __func__, __LINE__);
		return NTY_RESULT_FAILED;
	} 
	if (proto->u8ConnectFlag == 0) {
		ntylog("[%s:%s:%d] socket don't connect to server\n", __FILE__, __func__, __LINE__);
		return NTY_RESULT_FAILED;
	}
	
	buffer[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buffer[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ;	
	buffer[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_HEARTBEAT_REQ;	
#if 0
	*(C_DEVID*)(&buffer[NTY_PROTO_DEVID_IDX]) = proto->devid;
#else
	memcpy(buffer+NTY_PROTO_DEVID_IDX, &proto->selfId, sizeof(C_DEVID));
#endif
	length = NTY_PROTO_HEARTBEAT_REQ_CRC_IDX+sizeof(U32);
	
	n = ntySendFrame(nSocket, buffer, length);

	return n;
}

#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
void ntyProtoClientSetToken(void *_self, U8 *tokens, int length) {
	NattyProto *proto = _self;

	memcpy(proto->tokens, tokens, length);
	proto->tokenLen = (U16)length;
}

void ntyProtoClientSetPublishStatus(void *_self, U8 status) {
	NattyProto *proto = _self;

	proto->publishStatus = status;
}


#endif

/*
 * Login Packet
 * VERSION					1			BYTE
 * MESSAGE TYPE				1			BYTE (req, ack)
 * TYPE					1			BYTE 
 * DEVID					8			BYTE
 * ACKNUM					4			BYTE (Network Module Set Value)
 * CRC 					4			BYTE (Network Module Set Value)
 * 
 * send to server addr
 */
int ntyProtoClientLogin(void *_self) {
	NattyProto *proto = _self;
	int len;	
	U8 buffer[RECV_BUFFER_SIZE] = {0};	

	buffer[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buffer[NTY_PROTO_PROTOTYPE_IDX] = (U8) MSG_REQ;	
	buffer[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_LOGIN_REQ;
#if 0
	*(C_DEVID*)(&buffer[NTY_PROTO_DEVID_IDX]) = proto->devid;
#else
	memcpy(buffer+NTY_PROTO_LOGIN_REQ_DEVID_IDX, &proto->selfId, sizeof(C_DEVID));
#endif
	ntydbg("ntyProtoClientLogin %lld\n", proto->selfId);
#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
	memcpy(buffer+NTY_PROTO_LOGIN_REQ_JSON_LENGTH_IDX, &proto->tokenLen, sizeof(U16));
	memcpy(buffer+NTY_PROTO_LOGIN_REQ_JSON_CONTENT_IDX, &proto->tokens, proto->tokenLen);
	len = NTY_PROTO_LOGIN_REQ_JSON_CONTENT_IDX+proto->tokenLen+4;
#else
	len = NTY_PROTO_LOGIN_REQ_CRC_IDX+sizeof(U32);		
#endif
		

	ntylog(" ntyProtoClientLogin %d\n", __LINE__);
	ClientSocket *nSocket = ntyNetworkInstance();
	return ntySendFrame(nSocket, buffer, len);
}

int ntyProtoClientBind(void *_self, C_DEVID did, U8 *json, U16 length) {
	NattyProto *proto = _self;
	int len;	

	U8 buf[RECV_BUFFER_SIZE] = {0};	

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ;	
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_BIND_REQ;

	memcpy(buf+NTY_PROTO_BIND_APPID_IDX, &proto->selfId, sizeof(C_DEVID));
#if 0
	*(C_DEVID*)(&buf[NTY_PROTO_BIND_DEVICEID_IDX]) = did;
	len = NTY_PROTO_BIND_CRC_IDX + sizeof(U32);
#else
	memcpy(buf+NTY_PROTO_BIND_DEVICEID_IDX, &did, sizeof(C_DEVID));
	memcpy(buf+NTY_PROTO_BIND_JSON_LENGTH_IDX, &length, sizeof(U16));
	memcpy(buf+NTY_PROTO_BIND_JSON_CONTENT_IDX, json, length);

	len = length + NTY_PROTO_BIND_JSON_CONTENT_IDX + sizeof(U32);
#endif
	

	ntylog(" ntyProtoClientBind --> %lld", did);

	ClientSocket *nSocket = ntyNetworkInstance();
	return ntySendFrame(nSocket, buf, len);
}

int ntyProtoClientUnBind(void *_self, C_DEVID did) {
	NattyProto *proto = _self;
	int len;	

	U8 buf[NORMAL_BUFFER_SIZE] = {0};	

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ;	
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_UNBIND_REQ;

	memcpy(buf+NTY_PROTO_UNBIND_APPID_IDX, &proto->selfId, sizeof(C_DEVID));
	memcpy(buf+NTY_PROTO_UNBIND_DEVICEID_IDX, &did, sizeof(C_DEVID));
	//*(C_DEVID*)(&buf[NTY_PROTO_UNBIND_DEVICEID_IDX]) = did;
	len = NTY_PROTO_UNBIND_CRC_IDX + sizeof(U32);

	ClientSocket *nSocket = ntyNetworkInstance();
	return ntySendFrame(nSocket, buf, len);
}

int ntyProtoClientHeartBeat(void *_self) {
	NattyProto *proto = _self;
	int len;	

	U8 buf[NORMAL_BUFFER_SIZE] = {0};	

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ;	
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_HEARTBEAT_REQ;

	memcpy(buf+NTY_PROTO_HEARTBEAT_REQ_DEVID_IDX, &proto->selfId, sizeof(C_DEVID));
	//memcpy(buf+NTY_PROTO_UNBIND_DEVICEID_IDX, &did, sizeof(C_DEVID));
	//*(C_DEVID*)(&buf[NTY_PROTO_UNBIND_DEVICEID_IDX]) = did;
	len = NTY_PROTO_HEARTBEAT_REQ_CRC_IDX + sizeof(U32);

	ClientSocket *nSocket = ntyNetworkInstance();
	return ntySendFrame(nSocket, buf, len);
}



int ntyProtoClientLogout(void *_self) {
	NattyProto *proto = _self;
	int len;	
	U8 buf[NORMAL_BUFFER_SIZE] = {0};

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;	
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ;	
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_LOGOUT_REQ;
	memcpy(buf+NTY_PROTO_LOGOUT_REQ_DEVID_IDX, &proto->selfId, sizeof(C_DEVID));
	
	len = NTY_PROTO_LOGOUT_REQ_CRC_IDX+sizeof(U32);				

	ClientSocket *nSocket = ntyNetworkInstance();
	return ntySendFrame(nSocket, buf, len);
}

/*
 * 
 */
int ntyProtoClientVoiceReq(void *_self, U32 msgId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 
	
	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_VOICE_REQ;

	memcpy(&buf[NTY_PROTO_VOICE_REQ_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_VOICE_REQ_MSGID_IDX], &msgId, sizeof(U32));
	if (length != 0) {
		memcpy(&buf[NTY_PROTO_VOICE_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	}
	if (json != NULL) {
		memcpy(&buf[NTY_PROTO_VOICE_REQ_JSON_CONTENT_IDX], json, length);
	}

	length = NTY_PROTO_VOICE_REQ_JSON_CONTENT_IDX+length+4;

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

/*
 * @Param: void *_self, NattyProto Instance
 * @Param: U8 *json, json format data
 * @Param: U16 length, json length
 * 
 */
int ntyProtoClientVoiceAck(void *_self, U32 msgId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 
	
	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_VOICE_ACK;

	memcpy(&buf[NTY_PROTO_VOICE_ACK_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_VOICE_ACK_MSGID_IDX], &msgId, sizeof(U32));
	
	memcpy(&buf[NTY_PROTO_VOICE_ACK_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_VOICE_ACK_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_VOICE_ACK_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientVoiceDataReq(void *_self, C_DEVID gId, U8 *data, int length) {
	NattyProto *proto = _self;

	return ntySendBigBuffer(proto, data, length, gId);
}

int ntyProtoClientCommonReq(void *_self, C_DEVID gId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_COMMON_REQ;
	
	memcpy(&buf[NTY_PROTO_COMMON_REQ_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_COMMON_REQ_RECVID_IDX], &gId, sizeof(C_DEVID));

	memcpy(&buf[NTY_PROTO_COMMON_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_COMMON_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_COMMON_REQ_JSON_CONTENT_IDX+length+sizeof(U32);
	
	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientLocationReq(void *_self, C_DEVID gId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_ASYNCREQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_LOCATION_ASYNCREQ;

	memcpy(&buf[NTY_PROTO_LOCATION_ASYNC_REQ_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_LOCATION_ASYNC_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_LOCATION_ASYNC_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_LOCATION_ASYNC_REQ_JSON_CONTENT_IDX + length + sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientWeatherReq(void *_self, C_DEVID gId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_ASYNCREQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_WEATHER_ASYNCREQ;

	memcpy(&buf[NTY_PROTO_WEATHER_ASYNC_REQ_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_WEATHER_ASYNC_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_WEATHER_ASYNC_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_LOCATION_ASYNC_REQ_JSON_CONTENT_IDX + length + sizeof(U32);
	
	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientICCIDReq(void *_self, C_DEVID gId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_ASYNCREQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_ICCID_REQ;

	memcpy(&buf[NTY_PROTO_ICCID_REQ_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_ICCID_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_ICCID_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_ICCID_ACK_JSON_CONTENT_IDX + length + sizeof(U32);
	
	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientCommonAck(void *_self, U32 msgId, U8 *json, U16 length) {
	NattyProto *proto = _self;

	U8 buf[RECV_BUFFER_SIZE] = {0};
	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_COMMON_ACK;

	memcpy(&buf[NTY_PROTO_COMMON_ACK_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_COMMON_ACK_MSGID_IDX], &msgId, sizeof(U32));
	memcpy(&buf[NTY_PROTO_COMMON_ACK_JSON_LENGTH_IDX], &length, sizeof(U16));

	memcpy(&buf[NTY_PROTO_COMMON_ACK_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_COMMON_ACK_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientOfflineMsgReq(void *_self) {
	NattyProto *proto = _self;
	int len = 0;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_OFFLINE_MSG_REQ;

	memcpy(&buf[NTY_PROTO_OFFLINE_MSG_REQ_DEVICEID_IDX], &proto->selfId, sizeof(C_DEVID));
	len = NTY_PROTO_OFFLINE_MSG_REQ_CRC_IDX+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, len);	
	
}

int ntyProtoClientDataRoute(void *_self, C_DEVID toId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_ROUTE; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_DATA_ROUTE;

	memcpy(&buf[NTY_PROTO_DATA_ROUTE_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_DATA_ROUTE_RECVID_IDX], &toId, sizeof(C_DEVID));

	memcpy(&buf[NTY_PROTO_DATA_ROUTE_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_DATA_ROUTE_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_DATA_ROUTE_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientConfirmReq(void *_self, C_DEVID proposerId, C_DEVID devId, U32 msgId, U8 *json, U16 length) {
	NattyProto *proto = _self;	
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_BIND_CONFIRM_REQ;

	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_ADMIN_SELFID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_DEVICEID_IDX], &devId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_PROPOSER_IDX], &proposerId, sizeof(C_DEVID));

	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_MSGID_IDX], &msgId, sizeof(U32));
	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_BIND_CONFIRM_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_BIND_CONFIRM_REQ_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}


int ntyProtoClientUserDataPacketReq(void *_self, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_DATAPACKET_REQ;

	memcpy(&buf[NTY_PROTO_USERDATA_PACKET_REQ_DEVICEID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_USERDATA_PACKET_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_USERDATA_PACKET_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_USERDATA_PACKET_REQ_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();

	return ntySendFrame(pNetwork, buf, length);
}

int ntyProtoClientPerformancePacketReq(void *_self, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_REQ; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_PERFORMANCE_REQ;

	memcpy(&buf[NTY_PROTO_PERFORMANCE_PACKET_REQ_DEVICEID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_PERFORMANCE_PACKET_REQ_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_PERFORMANCE_PACKET_REQ_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_PERFORMANCE_PACKET_REQ_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();

	return ntySendFrame(pNetwork, buf, length);
}


int ntyProtoClientMsgPushPacketReq(void *_self, C_DEVID toId, U8 *json, U16 length) {
	NattyProto *proto = _self;
	U8 buf[RECV_BUFFER_SIZE] = {0}; 

	buf[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
	buf[NTY_PROTO_PROTOTYPE_IDX] = (U8) PROTO_ROUTE; 
	buf[NTY_PROTO_MSGTYPE_IDX] = NTY_PROTO_MSG_PUSH_REQ;

	memcpy(&buf[NTY_PROTO_MSG_PUSH_DEVID_IDX], &proto->selfId, sizeof(C_DEVID));
	memcpy(&buf[NTY_PROTO_MSG_PUSH_RECVID_IDX], &toId, sizeof(C_DEVID));

	memcpy(&buf[NTY_PROTO_MSG_PUSH_JSON_LENGTH_IDX], &length, sizeof(U16));
	memcpy(&buf[NTY_PROTO_MSG_PUSH_JSON_CONTENT_IDX], json, length);

	length = NTY_PROTO_MSG_PUSH_JSON_CONTENT_IDX+length+sizeof(U32);

	void *pNetwork = ntyNetworkInstance();
	return ntySendFrame(pNetwork, buf, length);
}



static const NattyProtoHandle ntyProtoOpera = {
	sizeof(NattyProto),
	ntyProtoClientCtor,
	ntyProtoClientDtor,
	ntyProtoClientLogin,
	ntyProtoClientLogout,

	ntyProtoClientVoiceReq,
	ntyProtoClientVoiceAck,
	ntyProtoClientVoiceDataReq,
	ntyProtoClientCommonReq,
	ntyProtoClientCommonAck,
	ntyProtoClientOfflineMsgReq,
	ntyProtoClientDataRoute,
	
	ntyProtoClientBind,
	ntyProtoClientUnBind,
	ntyProtoClientConfirmReq,
	
	ntyProtoClientUserDataPacketReq,
	ntyProtoClientPerformancePacketReq,
	ntyProtoClientMsgPushPacketReq,
};

const void *pNattyProtoOpera = &ntyProtoOpera;

static NattyProto *pProtoOpera = NULL;

void *ntyProtoInstance(void) { //Singleton
	if (pProtoOpera == NULL) {
		pProtoOpera = New(pNattyProtoOpera);
		if (pProtoOpera->u8ConnectFlag == 0) { //Socket Connect Failed
			Delete(pProtoOpera);
			pProtoOpera = NULL;
		}
	}
	return pProtoOpera;
}

static void *ntyProtoGetInstance(void) {
	return pProtoOpera;
}

void ntyProtoRelease(void) {
	if (pProtoOpera != NULL) {
		Delete(pProtoOpera);
		pProtoOpera = NULL;
	}
}


static void ntySetupRecvProcThread(void *self) {
	//NattyProtoOpera * const * protoOpera = self;
	NattyProto *proto = self;
	int err;
	//pthread_t recvThread_id;

	if (self && proto && proto->onRecvCallback) {	
		if (proto->recvThreadId != 0) {
			ntydbg(" recv thread is running \n");
			return ;
		}
		
		err = pthread_create(&proto->recvThreadId, NULL, proto->onRecvCallback, self);				
		if (err != 0) { 				
			ntydbg(" can't create thread:%s\n", strerror(err)); 
			return ;	
		}
	}
}

static int ntySendLogin(void *self) {
	NattyProtoOpera * const * protoOpera = self;

	ntydbg(" ntySendLogin %d\n", __LINE__);
	if (self && (*protoOpera) && (*protoOpera)->login) {
		return (*protoOpera)->login(self);
	}
}

static int ntySendLogout(void *self) {
	NattyProtoOpera * const * protoOpera = self;

	if (self && (*protoOpera) && (*protoOpera)->logout) {
		return (*protoOpera)->logout(self);
	}
}

void ntySetSendSuccessCallback(PROXY_CALLBACK cb) {
	onProxySuccess = cb;
}

void ntySetSendFailedCallback(PROXY_CALLBACK cb) {
	onProxyFailed = cb;
}

void ntySetProxyCallback(PROXY_CALLBACK cb) {
	onProxyCallback = cb;
}

void ntySetProxyDisconnect(PROXY_CALLBACK cb) {
	onProxyDisconnect = cb;
}

void ntySetProxyReconnect(PROXY_CALLBACK cb) {
	onProxyReconnect = cb;
}

void ntySetBindResult(PROXY_CALLBACK cb) {
	onBindResult = cb;
}

void ntySetUnBindResult(PROXY_CALLBACK cb) {
	onUnBindResult = cb;
}

void ntySetPacketRecv(NTY_PACKET_CALLBACK cb) {
	onPacketRecv = cb;
}

void ntySetPacketSuccess(PROXY_CALLBACK cb) {
	onPacketSuccess = cb;
}


void ntySetDevId(C_DEVID id) {
	gSelfId = id;
}

void ntySetLoginAckResult(NTY_PARAM_CALLBACK cb) {
	onLoginAckResult = cb;
}

void ntySetHeartBeatAckResult(NTY_STATUS_CALLBACK cb) {
	onHeartBeatAckResult = cb;
}

void ntySetLogoutAckResult(NTY_STATUS_CALLBACK cb) {
	onLogoutAckResult = cb;
}

void ntySetTimeAckResult(NTY_PARAM_CALLBACK cb) {
	onTimeAckResult = cb;
}

void ntySetICCIDAckResult(NTY_PARAM_CALLBACK cb) {
	onICCIDAckResult = cb;
}

void ntySetCommonReqResult(NTY_RETURN_CALLBACK cb) {
	onCommonReqResult = cb;
}

void ntySetVoiceDataAckResult(NTY_STATUS_CALLBACK cb) {
	onVoiceDataAckResult = cb;
}

void ntySetOfflineMsgAckResult(NTY_PARAM_CALLBACK cb) {
	onOfflineMsgAckResult = cb;
}

void ntySetLocationPushResult(NTY_PARAM_CALLBACK cb) {
	onLocationPushResult = cb;
}

void ntySetWeatherPushResult(NTY_PARAM_CALLBACK cb) {
	onWeatherPushResult = cb;
}

void ntySetDataRoute(NTY_RETURN_CALLBACK cb) {
	onDataRoute = cb;
}

void ntySetDataResult(NTY_PARAM_CALLBACK cb) {
	onDataResult = cb;
}

void ntySetVoiceBroadCastResult(NTY_RETURN_CALLBACK cb) {
	onVoiceBroadCastResult = cb;
}

void ntySetLocationBroadCastResult(NTY_RETURN_CALLBACK cb) {
	onLocationBroadCastResult = cb;
}

void ntySetCommonBroadCastResult(NTY_RETURN_CALLBACK cb) {
	onCommonBroadCastResult = cb;
}

void ntySetBindConfirmResult(NTY_RETURN_CALLBACK cb) {
	onBindConfirmResult = cb;
}

void ntySetUserDataAckResult(NTY_PARAM_CALLBACK cb) {
	onUserDataAck = cb;
}


int ntyCheckProtoClientStatus(void) {
	
#if 0
	if (onProxyCallback == NULL) return -2;
	if (onProxyFailed == NULL) return -3;
	if (onProxySuccess == NULL) return -4;
#endif
	if (onProxyDisconnect == NULL) return -5;
	if (onProxyReconnect == NULL) return -6;
	if (onBindResult == NULL) return -7;
	if (onUnBindResult == NULL) return -8;
	if (onRecvCallback == NULL) return -9;
	if (onPacketRecv == NULL) return -10;
	if (onPacketSuccess == NULL) return -11;
	
}

#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_IOS)
void ntySetIosTokenClient(U8 *iosTokens, int length) {
	memcpy(tokens, iosTokens, length);
	tokenLen = (U16)length;
}

void ntySetIosAppPublishStatus(U8 status) { //status: 0 -> development, 1 -> product
	if (status) {
		publishStatus = NTY_PROTO_CLIENT_IOS_PUBLISH;
	} else {
		publishStatus = NTY_PROTO_CLIENT_IOS;
	}
}

#endif


static int ntyReconnectCb(NITIMER_ID id, void *user_data, int len) {
	int status = 0;
	
	//trace(" ntyReconnectCb ...\n");
	NattyProto *proto = ntyProtoGetInstance();
	if (proto != NULL) {
		if (proto->u8ConnectFlag) {
			return 0;
		}
	}
	
	proto = ntyStartupClient(&status);
	if (status != -1 && (proto != NULL)) {
		ntylog(" ntyReconnectCb  Success... status:%d, flag:%d\n", status, proto->u8ConnectFlag);
		if (proto->u8ConnectFlag == 1) { //Reconnect Success
			proto->u8ConnectFlag = 2;
			
			//sleep(200 * 1000);
#if 1 //Stop Timer
			ntylog(" Stop Timer\n");
			void *nTimerList = ntyTimerInstance();
			ntyTimerDel(nTimerList, nReconnectTimer);
			nReconnectTimer = NULL;
#endif
			//if (proto->onProxyReconnect)
			//	proto->onProxyReconnect(0);
		}
	}

	return NTY_RESULT_SUCCESS;
}


void* ntyStartupClient(int *status) {
	NattyProto* proto = ntyProtoInstance();
	if (proto) {
		ntySendLogin(proto);
		ntySetupRecvProcThread(proto); //setup recv proc
		
		*status = proto->u8ConnectFlag;
	} else {
		*status = -1;
	}

	if (nReconnectTimer == NULL) {
		// startup failed
		void *nTimerList = ntyTimerInstance();
		nReconnectTimer = ntyTimerAdd(nTimerList, NTY_RECONNECTION_TIMER_TICK, ntyReconnectCb, NULL, 0);
	}

	return proto;
}

void ntyShutdownClient(void) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		ntySendLogout(proto);
		proto->u8RecvExitFlag = 2;
	}
	
	if (nReconnectTimer != NULL) {
		// shutdown failed
		void *nTimerList = ntyTimerInstance();
		ntyTimerDel(nTimerList, nReconnectTimer);
		nReconnectTimer = NULL;
	}
}

#if 1

int ntyBindClient(C_DEVID did, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientBind(proto, did, json, length);
	}
	return -1;
}

int ntyUnBindClient(C_DEVID did) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientUnBind(proto, did);
	}
	return -1;
}

int ntyVoiceReqClient(U32 msgId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientVoiceReq(proto, msgId, json, length);
	}
	return -1;
}

int ntyVoiceAckClient(U32 msgId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientVoiceAck(proto, msgId, json, length);
	}
	return -1;
}

int ntyVoiceDataReqClient(C_DEVID gId, U8 *data, int length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientVoiceDataReq(proto, gId, data, length);
	}
	return -1;
}

int ntyCommonReqClient(C_DEVID gId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientCommonReq(proto, gId, json, length);
	}
	return -1;
}

int ntyLocationReqClient(C_DEVID gId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientLocationReq(proto, gId, json, length);
	}
	return -1;
}

int ntyWeatherReqClient(C_DEVID gId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientWeatherReq(proto, gId, json, length);
	}
	return -1;
}

int ntyICCIDReqClient(C_DEVID gId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientICCIDReq(proto, gId, json, length);
	}
	return -1;
}

int ntyCommonAckClient(U32 msgId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return ntyProtoClientCommonAck(proto, msgId, json, length);
	}
	return -1;
}

int ntyDataRouteClient(C_DEVID toId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientDataRoute(proto, toId, json, length);
	}
	return -1;
}

int ntyBindConfirmReqClient(C_DEVID proposerId, C_DEVID devId, U32 msgId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientConfirmReq(proto, proposerId, devId, msgId, json, length);
	}
	return -1;
}

int ntyOfflineMsgReqClient(void) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientOfflineMsgReq(proto);
	}
	return -1;
}

int ntyUserDataReqClient(U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientUserDataPacketReq(proto, json, length);
	}
	return -1;
}

int ntyPerformReqClient(U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientPerformancePacketReq(proto, json, length);
	}
	return -1;
}


int ntyMsgPushReqClient(C_DEVID toId, U8 *json, U16 length) {
	NattyProto* proto = ntyProtoGetInstance();

	if (proto) {
		return ntyProtoClientMsgPushPacketReq(proto, toId, json, length);
	}
	return -1;
}


#endif

U8* ntyGetRecvBuffer(void) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		proto->recvBuffer[NTY_PROTO_DATAPACKET_CONTENT_IDX+proto->recvLen] = 0x0;
		return proto->recvBuffer+NTY_PROTO_DATAPACKET_CONTENT_IDX;
	}
	return NULL;
}

int ntyGetRecvBufferSize(void) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto) {
		return proto->recvLen;
	}
	return -1;
}




void ntyStartReconnectTimer(void) {
//disconnect 
	ntylog(" setup ntyStartReconnectTimer \n");
	if (nReconnectTimer == NULL) {
		ntylog(" nReconnectTimer == NULL \n");
		void *nTimerList = ntyTimerInstance();
		nReconnectTimer = ntyTimerAdd(nTimerList, NTY_RECONNECTION_TIMER_TICK, ntyReconnectCb, NULL, 0);
	}
}

char* ntyProtoClientGetVersion(void) {
	return sdk_version;
}


#if 1


int u32DataLength = 0;
static U8 u8RecvBigBuffer[NTY_BIGBUFFER_SIZE] = {0};
static U8 u8SendBigBuffer[NTY_BIGBUFFER_SIZE] = {0};
void* tBigTimer = NULL;
void* tEfenceTimer = NULL;
void* tProxyTimer = NULL;


int ntySendVoicePacket(void *self, U8 *buffer, int length, C_DEVID toId) {
	U16 Count = length / NTY_VOICEREQ_PACKET_LENGTH + 1 ;
	U32 pktLength = NTY_VOICEREQ_DATA_LENGTH, i;
	U8 *pkt = buffer;
	void *pNetwork = ntyNetworkInstance();
	NattyProto *proto = self;
	int ret = -1;

	LOG(" destId:%d, pktIndex:%d, pktTotal:%d, selfId:%lld", NTY_PROTO_VOICE_DATA_REQ_GROUP_IDX,
			NTY_PROTO_VOICE_DATA_REQ_PKTINDEX_IDX, NTY_PROTO_VOICE_DATA_REQ_PKTTOTLE_IDX, proto->selfId);
	
	for (i = 0;i < Count;i ++) {
		pkt = buffer+(i*NTY_VOICEREQ_PACKET_LENGTH);

		pkt[NTY_PROTO_VERSION_IDX] = NTY_PROTO_VERSION;
		pkt[NTY_PROTO_DEVTYPE_IDX] = NTY_PROTO_CLIENT_ANDROID;
		pkt[NTY_PROTO_PROTOTYPE_IDX] = (U8) MSG_REQ;	
		pkt[NTY_PROTO_VOICEREQ_TYPE_IDX] = NTY_PROTO_VOICE_DATA_REQ;

		memcpy(pkt+NTY_PROTO_VOICE_DATA_REQ_DEVID_IDX, &proto->selfId, sizeof(C_DEVID));
		memcpy(pkt+NTY_PROTO_VOICE_DATA_REQ_GROUP_IDX, &toId, sizeof(C_DEVID));

		memcpy(pkt+NTY_PROTO_VOICE_DATA_REQ_PKTINDEX_IDX, &i, sizeof(U16));
		memcpy(pkt+NTY_PROTO_VOICE_DATA_REQ_PKTTOTLE_IDX, &Count , sizeof(U16));

		if (i == Count-1) { //last packet
			pktLength = (length % NTY_VOICEREQ_PACKET_LENGTH) - NTY_VOICEREQ_EXTEND_LENGTH;

#if 1 //Cancel Timer
			void *nTimerList = ntyTimerInstance();

			if (nBigBufferSendTimer != NULL) {
				ntyTimerDel(nTimerList, nBigBufferSendTimer);
				nBigBufferSendTimer = NULL;
			}
#endif
		}

		memcpy(pkt+NTY_PROTO_VOICE_DATA_REQ_PKTLENGTH_IDX, &pktLength, sizeof(U32));
		
		ret = ntySendFrame(pNetwork, pkt, pktLength+NTY_VOICEREQ_EXTEND_LENGTH);

		LOG(" index : %d", i );
		LOG(" pktLength:%d, Count:%d, ret:%d, selfIdx:%d\n",
			pktLength+NTY_VOICEREQ_EXTEND_LENGTH, Count, ret, NTY_PROTO_VOICEREQ_SELFID_IDX);

		usleep(200 * 1000); //Window Send
	}

	return 0;
}

int ntyAudioPacketEncode(U8 *pBuffer, int length) {
	int i = 0, j = 0, k = 0, idx = 0;
	int pktCount = length / NTY_VOICEREQ_DATA_LENGTH;
	int pktLength = pktCount * NTY_VOICEREQ_PACKET_LENGTH + (length % NTY_VOICEREQ_DATA_LENGTH) + NTY_VOICEREQ_EXTEND_LENGTH;
	//U8 *pktIndex = pBuffer + pktCount * NTY_VOICEREQ_PACKET_LENGTH;

	LOG("pktLength :%d, pktCount:%d\n", pktLength, pktCount);
	if (pktCount >= (NTY_VOICEREQ_COUNT_LENGTH-1)) return -1;

	j = pktLength - NTY_CRCNUM_LENGTH;
	k = length;

	LOG("j :%d, k :%d, pktCount:%d, last:%d\n", j, k, pktCount, (length % NTY_VOICEREQ_DATA_LENGTH));
	for (idx = 0;idx < (length % NTY_VOICEREQ_DATA_LENGTH);idx ++) {
		pBuffer[--j] = pBuffer[--k];
	}	
			

	for (i = pktCount;i > 0;i --) {
		j = i * NTY_VOICEREQ_PACKET_LENGTH - NTY_CRCNUM_LENGTH;
		k = i * NTY_VOICEREQ_DATA_LENGTH;
		LOG("j :%d, k :%d\n", j, k);
		for (idx = 0;idx < NTY_VOICEREQ_DATA_LENGTH;idx ++) {
			pBuffer[--j] = pBuffer[--k];
		}
	}

	return pktLength;
}

int ntyGetRecvBigLength(void) {
	return u32DataLength;
}

U8 *ntyGetRecvBigBuffer(void) {
	return u8RecvBigBuffer;
}

U8 *ntyGetSendBigBuffer(void) {
	return u8SendBigBuffer;
}

static int ntySendBigBufferCb(NITIMER_ID id, void *user_data, int len) {
	NattyProto* proto = ntyProtoGetInstance();
	if (proto && proto->onPacketSuccess) {
		proto->onPacketSuccess(1); //Failed
#if 0
		if (tBigTimer != -1) {
			del_timer(tBigTimer);
			tBigTimer = -1;
		}
#else
		if (nBigBufferSendTimer != NULL) {
			void *nTimerList = ntyTimerInstance();
			
			ntyTimerDel(nTimerList, nBigBufferSendTimer);
			nBigBufferSendTimer = NULL;
		}
#endif
	}

	return 0;
}

static int ntySendBigBuffer(void *self, U8 *u8Buffer, int length, C_DEVID gId) {
	int i = 0;
	NattyProto *proto = self;
#if 0
	tBigTimer = add_timer(10, ntySendBigBufferCb, NULL, 0);
#else
	void *nTimerList = ntyTimerInstance();
	if (nBigBufferSendTimer == NULL) {
		nBigBufferSendTimer = ntyTimerAdd(nTimerList, PACKET_SEND_TIME_TICK, ntySendBigBufferCb, NULL, 0);
	}
#endif
	length = ntyAudioPacketEncode(u8Buffer, length);
	LOG(" ntySendBigBuffer --> Ret %d, %x, %lld, self:%lld", length, u8Buffer[0], gId, proto->selfId);

	ntySendVoicePacket(proto, u8Buffer, length, gId);
#if 0
	C_DEVID tToId = 0;
	memcpy(&tToId, u8Buffer+NTY_PROTO_VOICEREQ_DESTID_IDX, sizeof(C_DEVID));
	LOG(" ntySendBigBuffer --> toId : %lld, %d", tToId, NTY_PROTO_VOICEREQ_DESTID_IDX);
#endif
	return length;
}

int ntyAudioRecodeDepacket(U8 *buffer, int length) {
	int i = 0;
	U8 *pBuffer = ntyGetRecvBigBuffer();	
	U16 index = ntyU8ArrayToU16(buffer+NTY_PROTO_VOICEREQ_PKTINDEX_IDX);
	U16 Count = ntyU8ArrayToU16(&buffer[NTY_PROTO_VOICEREQ_PKTTOTLE_IDX]);
	U32 pktLength = ntyU8ArrayToU32(&buffer[NTY_PROTO_VOICEREQ_PKTLENGTH_IDX]);

	//nty_printf(" Count:%d, index:%d, pktLength:%d, length:%d, pktLength%d\n", 
	//				Count, index, pktLength, length, NTY_PROTO_VOICEREQ_PKTLENGTH_IDX);

	if (length != pktLength+NTY_VOICEREQ_EXTEND_LENGTH) return 2;

	
	for (i = 0;i < pktLength;i ++) {
		pBuffer[index * NTY_VOICEREQ_DATA_LENGTH + i] = buffer[NTY_VOICEREQ_HEADER_LENGTH + i];
	}

	if (index == Count-1) {
		u32DataLength = NTY_VOICEREQ_DATA_LENGTH*(Count-1) + pktLength;
		return 1;
	}

	return 0;
}


void ntyPacketClassifier(void *arg, U8 *buf, int length) {
	NattyProto *proto = arg;
	NattyProtoOpera * const *protoOpera = arg;
	Network *pNetwork = ntyNetworkInstance();
	U8 MSG = buf[NTY_PROTO_MSGTYPE_IDX];
	buf[length-NTY_CRCNUM_LENGTH] = 0x0;

	switch (MSG) {
		case NTY_PROTO_LOGIN_ACK: {

			U16 status = *(U16*)(buf+NTY_PROTO_LOGIN_ACK_STATUS_IDX);
			U16 jsonLen = *(U16*)(buf+NTY_PROTO_LOGIN_ACK_JSON_LENGTH_IDX);
			U8 *json = buf+NTY_PROTO_LOGIN_ACK_JSON_CONTENT_IDX;

			if (NTY_RESULT_FAILED == ntyOfflineMsgReqClient()) break;

			LOG(" LoginAckResult status:%d\n", status);
			if (proto->onLoginAckResult) {
				proto->onLoginAckResult(json, jsonLen);
			}
			break;
		} 
		case NTY_PROTO_BIND_ACK: {

			int result = 0;
			memcpy(&result, &buf[NTY_PROTO_BIND_ACK_RESULT_IDX], sizeof(int));

			if (proto->onBindResult) {
				proto->onBindResult(result);
			}
			ntydbg(" NTY_PROTO_BIND_ACK\n");
			break;
		} 
		case NTY_PROTO_UNBIND_ACK: {

			int result = 0;
			memcpy(&result, &buf[NTY_PROTO_BIND_ACK_RESULT_IDX], sizeof(int));

			if (proto->onUnBindResult) {
				proto->onUnBindResult(result);
			}
			ntydbg(" NTY_PROTO_UNBIND_ACK\n");
			break;
		} 
		case NTY_PROTO_HEARTBEAT_ACK: {
			
			break;
		} 
		case NTY_PROTO_LOGOUT_ACK: {
			
			break;
		}
#if (NTY_PROTO_SELFTYPE == NTY_PROTO_CLIENT_WATCH)
		case NTY_PROTO_TIME_CHECK_ACK: {
			//set system time
			break;
		}
		case NTY_PROTO_ICCID_ACK: {
			U16 status = 0;
			U16 u16Length = 0;
			U8 *json = NULL;

			memcpy(&status, buf+NTY_PROTO_ICCID_ACK_STATUS_IDX, sizeof(U16));
			memcpy(&u16Length, buf+NTY_PROTO_ICCID_ACK_JSON_LENGTH_IDX, sizeof(U16));
			json = buf+NTY_PROTO_ICCID_ACK_JSON_CONTENT_IDX;

			if (proto->onICCIDAckResult) {
				proto->onICCIDAckResult(json, u16Length);
			}
			
			break;
		}
#endif
		case NTY_PROTO_COMMON_REQ: {
			C_DEVID fromId = 0;
			U16 u16Length = 0;
			U8 *json = NULL;

			ntyU8ArrayToU64(buf+NTY_PROTO_COMMON_REQ_DEVID_IDX, &fromId);
			memcpy(&u16Length, buf+NTY_PROTO_COMMON_REQ_JSON_LENGTH_IDX, sizeof(U16));

			json = buf+NTY_PROTO_COMMON_REQ_JSON_CONTENT_IDX;

			if (proto->onCommonReqResult) {
				proto->onCommonReqResult(fromId, json, u16Length);
			}
			
			break;
		}
		case NTY_PROTO_VOICE_DATA_REQ: {
			int ret = ntyAudioRecodeDepacket(buf, length);
			if (ret == 1) {
				U32 u32MsgId = 0;
				C_DEVID fromId = 0;
				C_DEVID gId = 0;
				
				ntyU8ArrayToU64(buf+NTY_PROTO_VOICE_DATA_REQ_DEVID_IDX, &fromId);
				ntyU8ArrayToU64(buf+NTY_PROTO_VOICE_DATA_REQ_GROUP_IDX, &gId);
				memcpy(&u32MsgId, buf+NTY_PROTO_VOICE_DATA_REQ_OFFLINEMSGID_IDX, sizeof(U32));

				if (NTY_RESULT_FAILED == ntyVoiceAckClient(u32MsgId, NULL, 0)) {
					//Server Disconnect
					break;
				}

				if (proto->onPacketRecv) {
					proto->onPacketRecv(fromId, gId, u32DataLength);
				}
			}
			break;
		}
		case NTY_PROTO_VOICE_DATA_ACK: {
			U16 status = 0;

			memcpy(&status, buf+NTY_PROTO_VOICE_DATA_ACK_STATUS_IDX, sizeof(U16));

			//send voice data success
			if (proto->onVoiceDataAckResult) {
				proto->onVoiceDataAckResult(status);
			}
			break;
		}
		case NTY_PROTO_OFFLINE_MSG_ACK: {

			U16 jsonLen = *(U16*)(buf+NTY_PROTO_OFFLINE_MSG_ACK_JSON_LENGTH_IDX);
			U8 *json = buf+NTY_PROTO_OFFLINE_MSG_ACK_JSON_CONTENT_IDX;

			if (proto->onOfflineMsgAckResult) {
				proto->onOfflineMsgAckResult(json, jsonLen);
			}
			
			break;
		}
		case NTY_PROTO_DATA_ROUTE: {
			C_DEVID fromId = 0;
			
			memcpy(&fromId, buf+NTY_PROTO_DATA_ROUTE_DEVID_IDX, sizeof(C_DEVID));

			U16 jsonLen = *(U16*)(buf+NTY_PROTO_DATA_ROUTE_JSON_LENGTH_IDX);
			U8 *json = buf+NTY_PROTO_DATA_ROUTE_JSON_CONTENT_IDX;

			if (proto->onDataRoute) {
				proto->onDataRoute(fromId, json, jsonLen);
			}
			
			break;
		}
		case NTY_PROTO_DATA_RESULT: {
			U16 status = 0;
			int ackNum = 0;
			U16 u16Length = 0;
						
			U8 *json = buf+NTY_PROTO_DATA_RESULT_JSON_CONTENT_IDX;
			memcpy(&u16Length, buf+NTY_PROTO_DATA_RESULT_JSON_LENGTH_IDX, sizeof(U16));
						
			memcpy(&status, buf+NTY_PROTO_DATA_RESULT_STATUS_IDX, sizeof(U16));
			memcpy(&ackNum, buf+NTY_PROTO_DATA_RESULT_ACKNUM_IDX, sizeof(U32));
			LOG("Data Result:%d\n", status);
		
			if (proto->onDataResult) {
				proto->onDataResult(json, u16Length);
			}
			
			break;
		}
		case NTY_PROTO_VOICE_BROADCAST: { //Recv Voice Notify
			C_DEVID fromId = 0;
			U8 *json = NULL;
			U16 length = 0;
			U32 msgId = 0;

			memcpy(&msgId, buf+NTY_PROTO_VOICE_BROADCAST_MSGID_IDX, sizeof(U32));
			memcpy(&fromId, buf+NTY_PROTO_VOICE_BROADCAST_DEVID_IDX, sizeof(C_DEVID));
			memcpy(&length, buf+NTY_PROTO_VOICE_BROADCAST_JSON_LENGTH_IDX, sizeof(U16));

			json = buf+NTY_PROTO_VOICE_BROADCAST_JSON_CONTENT_IDX;

			//send Voice Ack
			if (NTY_RESULT_FAILED == ntyVoiceReqClient(msgId, NULL, 0)) {
				//server disconnect
				break;
			}

			// voice data notify
			if (proto->onVoiceBroadCastResult) {
				proto->onVoiceBroadCastResult(fromId, json, length);
			}
			
			break;
		}
		case NTY_PROTO_LOCATION_BROADCAST: {
			C_DEVID fromId = 0;
			U8 *json = NULL;
			U16 u16Length = 0;
			
			memcpy(&fromId, buf+NTY_PROTO_LOCATION_BROADCAST_DEVID_IDX, sizeof(C_DEVID));
			memcpy(&u16Length, buf+NTY_PROTO_LOCATION_BROADCAST_JSON_LENGTH_IDX, sizeof(U16));

			json = buf+NTY_PROTO_LOCATION_BROADCAST_JSON_CONTENT_IDX;
			

			if (proto->onLocationBroadCastResult) {
				proto->onLocationBroadCastResult(fromId, json, u16Length);
			}
			break;
		}
		case NTY_PROTO_DATAPACKET_ACK: {
			U16 u16Length = 0;
			U8 *json = NULL;
			
			memcpy(&u16Length, buf+NTY_PROTO_USERDATA_PACKET_ACK_JSON_LENGTH_IDX, sizeof(U16));
			json = buf+NTY_PROTO_USERDATA_PACKET_ACK_JSON_CONTENT_IDX;
			if (proto->onUserDataAck) {
				proto->onUserDataAck(json, u16Length);
			}
			break;
		}
		case NTY_PROTO_COMMON_BROADCAST: {
			C_DEVID fromId = 0;
			U8 *json = NULL;
			U16 u16Length = 0;
			U32 u32MsgId = 0;
			
			memcpy(&fromId, buf+NTY_PROTO_COMMON_BROADCAST_DEVID_IDX, sizeof(C_DEVID));
			memcpy(&u32MsgId, buf+NTY_PROTO_COMMON_BROADCAST_MSGID_IDX, sizeof(U32));
			memcpy(&u16Length, buf+NTY_PROTO_COMMON_BROADCAST_JSON_LENGTH_IDX, sizeof(U16));

			json = buf+NTY_PROTO_COMMON_BROADCAST_JSON_CONTENT_IDX;

			if (NTY_RESULT_FAILED == ntyCommonAckClient(u32MsgId, NULL, 0)) {
				//Server Disconnect
				break;
			}
			
			if (proto->onCommonBroadCastResult) {
				proto->onCommonBroadCastResult(fromId, json, u16Length);
			}
			
			break;
		}
#if (NTY_PROTO_SELFTYPE == NTY_PROTO_CLIENT_WATCH)		
		case NTY_PROTO_LOCATION_PUSH: {
			U8 *json = NULL;
			U16 u16Length = 0;
			
			memcpy(&u16Length, buf+NTY_PROTO_LOCATION_PUSH_JSON_LENGTH_IDX, sizeof(U16));
			json = buf+NTY_PROTO_LOCATION_PUSH_JSON_CONTENT_IDX;
			
			if (proto->onLocationPushResult) {
				proto->onLocationPushResult(json, u16Length);
			}

			break;
		}
		case NTY_PROTO_WEATHER_PUSH: {

			U8 *json = NULL;
			U16 u16Length = 0;
			
			memcpy(&u16Length, buf+NTY_PROTO_WEATHER_PUSH_JSON_LENGTH_IDX, sizeof(U16));
			json = buf+NTY_PROTO_WEATHER_PUSH_JSON_CONTENT_IDX;

			if (proto->onWeatherPushResult) {
				proto->onWeatherPushResult(json, u16Length);
			}
			
			break;
		}
#endif		
#if (NTY_PROTO_SELFTYPE != NTY_PROTO_CLIENT_WATCH || NTY_PROTO_SELFTYPE != NTY_PROTO_SERVER) 	
		case NTY_PROTO_BIND_CONFIRM_PUSH: {
			U8 *json = NULL;
			U32 msgId = 0;
			C_DEVID proposerId = 0;
			C_DEVID devId = 0;
			U16 u16Length = 0;

			//
			memcpy(&proposerId, buf+NTY_PROTO_BIND_CONFIRM_PUSH_PROPOSER_IDX, sizeof(C_DEVID));
			memcpy(&devId, buf+NTY_PROTO_BIND_CONFIRM_PUSH_DEVICE_IDX, sizeof(C_DEVID));

			memcpy(&u16Length, buf+NTY_PROTO_BIND_CONFIRM_PUSH_JSON_LENGTH_IDX, sizeof(U16));
			
			json = buf+NTY_PROTO_BIND_CONFIRM_PUSH_JSON_CONTENT_IDX;

			if (proto->onBindConfirmResult) {
				proto->onBindConfirmResult(proposerId, json, u16Length);
			}
			
			break;
		}
#endif
	}
}

static U8 rBuffer[NTY_VOICEREQ_PACKET_LENGTH] = {0};
static U16 rLength = 0;
extern U32 ntyGenCrcValue(U8 *buf, int length);

int ntyPacketValidator(void *self, U8 *buffer, int length) {
	int bCopy = 0, bIndex = 0, ret = -1;
	U32 uCrc = 0, uClientCrc = 0;
	int bLength = length;
	
	uCrc = ntyGenCrcValue(buffer, length-4);
	uClientCrc = ntyU8ArrayToU32(buffer+length-4);
	LOG(" rLength :%d, length:%d, crc:%x, u8Crc:%x\n", rLength, length, uCrc, uClientCrc);

	if (uCrc != uClientCrc) {
		do {
			bCopy = (bLength > NTY_VOICEREQ_PACKET_LENGTH ? NTY_VOICEREQ_PACKET_LENGTH : bLength);
			bCopy = (((rLength + bCopy) > NTY_VOICEREQ_PACKET_LENGTH) ? (NTY_VOICEREQ_PACKET_LENGTH - rLength) : bCopy);
			
			memcpy(rBuffer+rLength, buffer+bIndex, bCopy);
			rLength += bCopy;
			
			uCrc = ntyGenCrcValue(rBuffer, rLength-4);
			uClientCrc = ntyU8ArrayToU32(rBuffer+rLength-4);

			LOG("uCrc:%x  uClientCrc:%x", uCrc, uClientCrc);
			if (uCrc == uClientCrc)	 {
				LOG(" CMD:%x, Version:[%d]\n", rBuffer[NTY_PROTO_MSGTYPE_IDX], rBuffer[NTY_PROTO_VERSION_IDX]);
				
				ntyPacketClassifier(self, rBuffer, rLength);

				rLength = 0;
				ret = 0;
			} 
			
			bLength -= bCopy;
			bIndex += bCopy;
			rLength %= NTY_VOICEREQ_PACKET_LENGTH;
			
		} while (bLength);
	} else  {
		ntyPacketClassifier(self, buffer, length);
		rLength = 0;
		ret = 0;
	}

	return ret;
}

#endif


static void* ntyRecvProc(void *arg) {
	struct sockaddr_in addr;
	int nSize = sizeof(struct sockaddr_in);
	NattyProto *proto = arg;
	NattyProtoOpera * const *protoOpera = arg;
	U8 *buf = proto->recvBuffer;

	int ret;
	void *pNetwork = ntyNetworkInstance();

	struct pollfd fds;
	fds.fd = ntyGetSocket(pNetwork);
	fds.events = POLLIN;

	ntydbg(" ntyRecvProc %d\n", fds.fd);
	while (1) {
		if (proto->u8RecvExitFlag == 1){ //reconnect
			ntydbg(" ntyRecvProc Reconnect Exist\n");
			ntyProtoRelease();

			ntyStartReconnectTimer();
			break;
		} else if (proto->u8RecvExitFlag == 2) {  //shutdown
			ntydbg(" ntyRecvProc Shutdown Exist\n");
			ntyProtoRelease();
			break;
		}
		
		ret = poll(&fds, 1, 5);
		if (ret) {
			bzero(buf, RECV_BUFFER_SIZE);
			proto->recvLen = ntyRecvFrame(pNetwork, buf, RECV_BUFFER_SIZE);
			if (proto->recvLen == 0) { //disconnect
#if 1				
				proto->u8RecvExitFlag = 1;
#endif
				
				ntydbg("Prepare to Reconnect to server\n");
				if (proto->onProxyDisconnect) {
					proto->onProxyDisconnect(0);
				}

				continue;
			} else if (proto->recvLen > RECV_BUFFER_SIZE) {
				proto->u8RecvExitFlag = 1;
				continue;
			}

			int nServFd = ntyGetSocket(pNetwork);
			getpeername(nServFd,(struct sockaddr*)&addr, &nSize);
			ntydbg("\n%d.%d.%d.%d:%d, length:%d --> %x, id:%lld\n", *(unsigned char*)(&addr.sin_addr.s_addr), *((unsigned char*)(&addr.sin_addr.s_addr)+1),													
				*((unsigned char*)(&addr.sin_addr.s_addr)+2), *((unsigned char*)(&addr.sin_addr.s_addr)+3),													
				addr.sin_port, proto->recvLen, buf[NTY_PROTO_MSGTYPE_IDX], *(C_DEVID*)(&buf[NTY_PROTO_DEVID_IDX]));	

			ntyPacketValidator(arg, buf, proto->recvLen);

		}
	}

#if 0		
	proto->u8RecvExitFlag = 0;
#endif


}






