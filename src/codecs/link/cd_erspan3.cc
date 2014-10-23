/*
** Copyright (C) 2002-2013 Sourcefire, Inc.
** Copyright (C) 1998-2002 Martin Roesch <roesch@sourcefire.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
// cd_erspan3.cc author Josh Rosenbaum <jrosenba@cisco.com>



#include <arpa/inet.h>
#include "framework/codec.h"
#include "codecs/codec_module.h"
#include "codecs/codec_events.h"
#include "protocols/protocol_ids.h"
#include "protocols/packet.h"

#define CD_ERSPAN3_NAME "erspan3"
#define CD_ERSPAN3_HELP "support for encapsulated remote switched port analyzer - type 3"

namespace
{

static const RuleMap erspan3_rules[] =
{
    { DECODE_ERSPAN3_DGRAM_LT_HDR, "captured < ERSpan type3 header length" },
    { 0, nullptr }
};

class Erspan3Module : public CodecModule
{
public:
    Erspan3Module() : CodecModule(CD_ERSPAN3_NAME, CD_ERSPAN3_HELP) {}

    const RuleMap* get_rules() const
    { return erspan3_rules; }
};


class Erspan3Codec : public Codec
{
public:
    Erspan3Codec() : Codec(CD_ERSPAN3_NAME){};
    ~Erspan3Codec(){};


    void get_protocol_ids(std::vector<uint16_t>& v) override;
    bool decode(const RawData&, CodecData&, DecodeData&) override;
};


struct ERSpanType3Hdr
{
    uint16_t ver_vlan;
    uint16_t flags_spanId;
    uint32_t time_stamp; // adding an underscore so function can be called timestamp()
    uint16_t pad0;
    uint16_t pad1;
    uint32_t pad2;
    uint32_t pad3;

    inline uint16_t version() const
    { return ntohs(ver_vlan) >> 12; }

    inline uint16_t vlan() const
    { return ntohs(ver_vlan) & 0xfff; }

    inline uint16_t span_id() const
    { return ntohs(flags_spanId) & 0x03ff; }

    inline uint32_t timestamp() const
    { return ntohs(time_stamp); }
};

const uint16_t ETHERTYPE_ERSPAN_TYPE3 = 0x22eb;
} // anonymous namespace

void Erspan3Codec::get_protocol_ids(std::vector<uint16_t>& v)
{
    v.push_back(ETHERTYPE_ERSPAN_TYPE3);
}


/*
 * Function: DecodeERSPANType3(uint8_t *, uint32_t, Packet *)
 *
 * Purpose: Decode Encapsulated Remote Switch Packet Analysis Type 3
 *          This will decode ERSPAN Type 3 Headers
 *
 * Arguments: pkt => ptr to the packet data
 *            len => length from here to the end of the packet
 *            p   => pointer to decoded packet struct
 *
 * Returns: void function
 *
 */
bool Erspan3Codec::decode(const RawData& raw, CodecData& codec, DecodeData&)
{
    const ERSpanType3Hdr* const erSpan3Hdr =
        reinterpret_cast<const ERSpanType3Hdr*>(raw.data);

    if (raw.len < sizeof(ERSpanType3Hdr))
    {
        codec_events::decoder_event(codec, DECODE_ERSPAN3_DGRAM_LT_HDR);
        return false;
    }

    /* Check that this is in fact ERSpan Type 3.
     */
    if (erSpan3Hdr->version() != 0x02) /* Type 3 == version 0x02 */
    {
        codec_events::decoder_event(codec, DECODE_ERSPAN_HDR_VERSION_MISMATCH);
        return false;
    }

    codec.next_prot_id = ETHERTYPE_TRANS_ETHER_BRIDGING;
    codec.lyr_len = sizeof(ERSpanType3Hdr);
    return true;
}

//-------------------------------------------------------------------------
// api
//-------------------------------------------------------------------------

static Module* mod_ctor()
{ return new Erspan3Module; }

static void mod_dtor(Module* m)
{ delete m; }

static Codec* ctor(Module*)
{ return new Erspan3Codec(); }

static void dtor(Codec *cd)
{ delete cd; }


static const CodecApi erspan3_api =
{
    {
        PT_CODEC,
        CD_ERSPAN3_NAME,
        CD_ERSPAN3_HELP,
        CDAPI_PLUGIN_V0,
        0,
        mod_ctor,
        mod_dtor,
    },
    nullptr, // pinit
    nullptr, // pterm
    nullptr, // tinit
    nullptr, // tterm
    ctor, // ctor
    dtor, // dtor
};



#ifdef BUILDING_SO
SO_PUBLIC const BaseApi* snort_plugins[] =
{
    &erspan3_api.base,
    nullptr
};
#else
const BaseApi* cd_erspan3 = &erspan3_api.base;
#endif
