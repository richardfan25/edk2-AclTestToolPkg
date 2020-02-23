###############################################################################
#
#  AclTestToolPkg - Advantech Test Tool Package
#
###############################################################################

===============================================================================
  Release Note
===============================================================================
1. Review and modify AdvEcPkg
2. EDK2 source tree is based on EDK2-UDK2018
3. It can be built in Linux enviroment

===============================================================================
  Application
===============================================================================
ectest2      : ITE EC test tool (ECTEST UEFI version)
ecutil       : Early version of EC flash tool

pchi2cdriver : PCH I2C driver
pchi2ctest   : PCH I2C test tool
pchsmbtest   : PCH SMBus test tool

powerup      : POWERUP (UEFI version)

rdccm        : RDC EC PMC communication tool
               - RD tool for sending RDC PMC protocol (support FW/SW-code base)
rdcdriver    : RDC EC PMC driver
rdci2c       : RDC I2C test tool
rdci2c_ex    : RDC I2C test tool ex
rdcsmbus     : RDC SMBus test tool
rdctest      : RDC EC test tool (like ECTEST for ITE EC)
rdcutil      : RDC EC utility (SW-code base test tool)

s3mon        : The S3 status detect utility
smarcgpio    : The GPIO pins test for SMARC
smbtest      : Early version of pchsmbtest, Chipset SMBus test utility

===============================================================================
  Build Option
===============================================================================
 -a : X64, IA32
 -b : RELEASE, DEBUG
 -t : GCC5, VS2012..
 -p : Package Name
 -m : Application in Package

 <none>   : at end of command line (build)
 cleanall : at end of command line (including meta files)
 clean    : at end of command line 

===============================================================================
  Quick Build
===============================================================================
Build an application (take few seconds)
$ build -a X64 -b RELEASE -t GCC5 -p AclTestToolPkg/AclTestToolPkg.dsc -m AclTestToolPkg/Applications/ectest2/ectest2.inf

Build a package (take a while)
$ build -a X64 -b RELEASE -t GCC5 -p AclTestToolPkg/AclTestToolPkg.dsc

Build all package in EDK2 (take a long time)
$ build -a X64 -b RELEASE -t GCC5

===============================================================================
  Result
===============================================================================
Output file will be generated at :
  $(TOP_DIR)/Build/AclTestToolPkg/RELEASE_GCC5/X64/

where $(TOP_DIR) is your EDK2 source tree directory

===============================================================================
  Reference
===============================================================================
1. EDK2 official site
   https://github.com/tianocore/edk2

2. EDK2 Getting Started
   https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II
