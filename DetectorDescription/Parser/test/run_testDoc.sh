#!/bin/bash
test=testtestDoc
function die { echo Failure $1: status $2 ; exit $2 ; }
pushd ${LOCAL_TMP_DIR}
  export mecpath=${PATH}
  export PATH=${RELEASETOP}/test/${SCRAM_ARCH}/:${LOCAL_TOP_DIR}/test/${SCRAM_ARCH}/:${PATH}
  cd ${LOCAL_TEST_DIR}
#  cp ${LOCAL_TOP_DIR}/test/${SCRAM_ARCH}/testDoc .
  echo ${test}testDoc ------------------------------------------------------------
  env
  testDoc testConfiguration.xml || die "testDoc" $?
  export PATH=${mecpath}
  popd
exit 0
