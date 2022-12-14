// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>

#include <string>

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#include <thread>
#include <memory>
#include <mutex>

#include <vector>
#include <list>

#include <deque>
#include <stack>
#include<queue>

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "Singleton.h"
#include "IOCP.h"
#include "IOCP_Message.h"
#include "AssemblyBuffer.h"
#include "Packet.h"
#include "Session.h"
#include "NetworkSystem.h"
#include "ServerSystem.h"

