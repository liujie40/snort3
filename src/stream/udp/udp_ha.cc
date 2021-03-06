//--------------------------------------------------------------------------
// Copyright (C) 2015-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// udp_ha.cc author Ed Borgoyn <eborgoyn@cisco.com>

#include "udp_ha.h"

#include "main/snort_debug.h"

void UdpHA::delete_session(Flow*)
{
    DebugMessage(DEBUG_HA,"UdpHA::delete_session)\n");
}

void UdpHA::create_session(Flow*)
{
    DebugMessage(DEBUG_HA,"UdpHA::create_session)\n");
}

THREAD_LOCAL UdpHA* UdpHAManager::udp_ha;

void UdpHAManager::tinit()
{
    udp_ha = new UdpHA();
}

void UdpHAManager::tterm()
{
    delete udp_ha;
}

