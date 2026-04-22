#ifndef __INTEGRATOR_SRV_TYPES_H__
#define __INTEGRATOR_SRV_TYPES_H__

#include "Common/BaseSrvTypes.h"

// Внутренние запросы
enum  eInterReqType {
  irUnknown = -1,
  irGetBillReq = 211,
  irGetBillResp = 212,
  irPayBillReq = 213,
  irPayBillResp = 214,
  irStornoBillReq = 215,
  irStornoBillResp = 216
};
/*
// Текущие интегрированные настройки для сервиса
struct S_INTER_CONFIG {
  std::string           idService;
  bool                  listmode;       // ` tinyint(4) DEFAULT '0' COMMENT '0 - списки не использ.',
  bool                  countSubSrv;    // ` tinyint(4) DEFAULT '0' COMMENT '0 - нет услуг',
  bool                  periodMode;     // ` tinyint(4) DEFAULT '0' COMMENT '0 - период не треб.',
  bool                  counterMode;    // ` tinyint(4) DEFAULT '0' COMMENT '0 - счетчики не использ.',
  std::string           billTags;       // ` varchar(24) DEFAULT 'AM|AP|CH2|MC|TT|UA',
  bool                  chequeMode;     // ` tinyint(4) DEFAULT '0' COMMENT '0 - чек формирует ТрейдСерв., 1 - чек дает Поставщик',
  bool                  repeatMode;     // ` tinyint(4) DEFAULT '0' COMMENT '1 - при повторе идти на предм.сервер'
  bool                  saveTrans;      // Сохранять таги в iTransactionComm
  bool                  useIdCompanyCheque; // Брать чек с учетом кода компании
};
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __INTEGRATOR_SRV_TYPES_H__
