#ifndef _RDCTESTSW_H__
#define _RDCTESTSW_H__

















void ShowBoardInformationSW(void);
void ShowHardwareMonitorSW(void);
void ThermalTestSW(void);
void WatchDogTestSW(void);
void FanTestSW(void);
void GPIOTestSW(void);
void PanelTestSW(void);
void CaseOpenTestSW(void);



int StorageSWProtocolGetStatus(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);
int StorageSWProtocolSetLockUnlock(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);
int StorageSWRWProtocol(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);
void StorageTestSW(void);


#endif
