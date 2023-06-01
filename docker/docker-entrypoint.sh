#!/bin/bash
declare -A array



array[-s]="${RTP_PROXY_PROTO:="udp"}:${RTP_PROXY_SOCKET:="0.0.0.0"}:${RTP_PROXY_SOCKET_PORT:="22223"}"
array[-l]="$RTP_PROXY_EXTERNAL_IP:$RTP_PROXY_INTERNAL_IP"
array[-A]="$RTP_PROXY_EXTERNAL_ALIAIS:$RTP_PROXY_INTERNAL_ALIAIS"
array[-m]=$RTP_PROXY_MIN_PORT
array[-M]=$RTP_PROXY_MAX_PORT
array[-u]=$RTP_PROXY_GNAME
array[-p]=$RTP_PROXY_PID_FILE
array[-L]=$RTP_PROXY_OPEN_FILE
array[-d]="$RTP_PROXY_LOG_LEVEL"
array[--config]=$RTP_PROXY_CONFIG_FILE


for i in "${!array[@]}"
do
    value="${array[$i]}"
    value=`echo $value | sed -e 's/^[[:space:]]*//'`
    if [[ -n "$value" ]]; then
    
        options+=" $i $value"
    fi
done

if [[ -n "$RTP_PROXY_CONFIG_FILE" ]]; then
    echo "modules {
       rtpp_acct_rtcp_hep {
           load = ../modules/acct_rtcp_hep/.libs/rtpp_acct_rtcp_hep.so
           capt_host  = ${HEP_IP}
           capt_port  = ${HEP_PORT}
           capt_ptype = ${HEP_PORTO}
           capt_id = ${HEP_ID}
       }
    }" > $RTP_PROXY_CONFIG_FILE
fi


useradd -m $RTP_PROXY_GNAME

/usr/local/bin/rtpproxy -f $options
