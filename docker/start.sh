#!/bin/bash
if [ -n "$RTP_PROXY_PROTO" ]; then  echo "$RTP_PROXY_PROTO:"; else  ${RTP_PROXY_PROTO}="udp:"; fi
if [[  -z ${RTP_PROXY_PROTO+x} ]] ;  then
  ABC_USAGE=$((ABC_USAGE+1))
fi
 echo $ABC_USAGE

echo ${RTP_PROXY_PROTO}