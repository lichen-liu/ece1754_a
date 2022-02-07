# ece1754_a

## To Run
```
cd MyFirstRosePass
make
./MyFirstRosePass testA.c
```

## Environment
```csh
setenv ROSE_ROOT /u/course/ece1754/rose
setenv ROSE_INSTALL ${ROSE_ROOT}/ROSE_INSTALL
setenv JAVA_HOME ${ROSE_ROOT}/jdk-17.0.1/
setenv LD_LIBRARY_PATH /usr/lib64
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${JAVA_HOME}/lib/server
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ROSE_INSTALL}/lib
```

```bash
export ROSE_ROOT=/u/course/ece1754/rose
export ROSE_INSTALL=${ROSE_ROOT}/ROSE_INSTALL
export JAVA_HOME=${ROSE_ROOT}/jdk-17.0.1/
export LD_LIBRARY_PATH=/usr/lib64
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${JAVA_HOME}/lib/server
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ROSE_INSTALL}/lib
```