#!/bin/sh
rm -fr cumulus
mkdir -p cumulus
#cp -a ../CumulusEdge/CumulusEdge cumulus/
cp -a ../CumulusServer/CumulusServer cumulus/
#cp -a ../CumulusLib/libCumulus.so cumulus/
#cp -a /usr/local/lib/libPocoFoundation.so* cumulus/
#cp -a /usr/local/lib/libPocoNet.so* cumulus/
#cp -a /usr/local/lib/libPocoXML.so* cumulus/
#cp -a /usr/local/lib/libPocoUtil.so* cumulus/

#cp -a /usr/local/lib64/libssl.so* cumulus/
#cp -a /usr/local/lib64/libcrypto.so* cumulus/
cp -a *.ini cumulus/
cp -a monitor*.sh cumulus/
cp -a *.lua cumulus/
tar czvf cumulus-`date '+%Y%m%d%H'`.tar.gz cumulus/
