#include <iostream>
#include "fsm.hpp"

FsmSimple::FsmSimple(std::string fsm_name_) : post_event(false),
                   warning_or_normal(false),
                   m_fsm_name(fsm_name_),
                   m_fsm_inited(false),
                   m_init_state(-1),
                   m_active_stId(-1),
                   m_preAct_stId(-1),
                   m_fsm_running(false),
                   m_active_state(nullptr) {
   m_all_stateUMap.clear();
}

bool FsmSimple::registerState(const int stateId, State *p_state) {
  auto ret = m_all_stateUMap.insert(std::make_pair(stateId, p_state));
  if(!ret.second) {
    std::cout << "current stateId is already registered" << std::endl;
    return false;
  }
  std::cout << "register state, id is " << stateId << std::endl;
  return true;
}

bool FsmSimple::setInitState(const int stateId) {
  if(m_all_stateUMap.find(stateId) == m_all_stateUMap.end()) {
    std::cout << "don't have this stateId";
    return false;
  }
  m_init_state = stateId;
  return true;
}

State* FsmSimple::getActiveState() {
  if(!m_fsm_running)
    return nullptr;
  return m_active_state;
}

bool FsmSimple::checkExist(const int stateId) {
  if(m_all_stateUMap.find(stateId) == m_all_stateUMap.end()) {
    std::cout << "current state is not register" << std::endl;
    return false;
  }
  return true;
}

State* FsmSimple::findStateById(const int stateId) {
  if(!checkExist(stateId)) return nullptr;
  return m_all_stateUMap[stateId];
}

const int FsmSimple::getPreStareId() {
  if(m_preAct_stId == -1) return false;
  return m_preAct_stId;
}

bool FsmSimple::tranState(const int stateId) {
  if(!m_fsm_running) {
    std::cout << "running == false" << std::endl;
    return false;
  }
  if(!checkExist(stateId)) return false;
  m_all_stateUMap.at(m_active_stId)->onStateExit();
  m_preAct_stId = m_active_stId;
  m_active_stId = stateId;
  m_active_state = m_all_stateUMap[m_active_stId];
  m_all_stateUMap.at(stateId)->onStateEnter();
  std::cout << "to do trans " << m_preAct_stId << "-->" << m_active_stId << std::endl;
  return true;
}

FSMManager::FSMManager() {
  m_all_FSMMap.clear();
  m_running_FSMSet.clear();
  event_in.clear();
  event_out.clear();
}


bool FSMManager::checkExist(const int FSMId) {
  if(m_all_FSMMap.find(FSMId) != m_all_FSMMap.end()) {
    std::cout << "current FSM "<< FSMId << " already register" << std::endl;
    return true;
  }
  return false;
}

bool FSMManager::registerFSM(const int FSMId, FsmSimple* FSM) {
  if(checkExist(FSMId)) return false;
  m_all_FSMMap.insert(std::make_pair(FSMId, FSM));
  return true;
}

bool FSMManager::unregisterFSM(const int FSMId) {
  if(!checkExist(FSMId)) return false;
  auto erase_size = m_all_FSMMap.erase(FSMId);
  if(erase_size == 0) return false;
  return true;
}

bool FSMManager::startFSM(const int FSMId) {
  if(!checkExist(FSMId)) return false;
  if(!m_all_FSMMap.at(FSMId)->start()) {
    std::cout << "start failed" << std::endl;
    return false;
  }
  m_running_FSMSet.insert(FSMId);
  std::cout << "success start " << FSMId << std::endl;
  return true;
}

bool FSMManager::stopFSM(const int FSMId) {
  if(!checkExist(FSMId)) return false;
  if(!m_all_FSMMap.at(FSMId)->stop()) return false;
  m_running_FSMSet.erase(FSMId);
  return true;
}

bool FSMManager::suspendFSM(const int FSMId) {
  if(!checkExist(FSMId)) return false;
  if(!m_all_FSMMap.at(FSMId)->suspend()) return false;
  m_running_FSMSet.insert(FSMId);
  return true;
}

bool FSMManager::resumeFSM(const int FSMId) {
  if(!checkExist(FSMId)) return false;
  if(!m_all_FSMMap.at(FSMId)->resume()) return false;
  m_running_FSMSet.insert(FSMId);
  return true;
}

bool FSMManager::onTick() {
  if(!dispatchEvent()) {
    std::cout << "dispatchEvent failed" << std::endl;
    return false;
  }
  if(!m_running_FSMSet.empty()) {
    for(auto &i : m_running_FSMSet) {
      m_all_FSMMap.at(i)->updateTime(m_time);
      m_all_FSMMap.at(i)->onTick();
    }
  }
  if(!postEvent()) {
    std::cout << "postEvent failed" << std::endl;
  }
  return true;
}

bool FSMManager::dispatchEvent() {
  if(event_in.empty()) {
    std::cout << "event in empty" << std::endl;
    return false;
  }
  for(auto iter = m_all_FSMMap.begin(); iter != m_all_FSMMap.end(); iter++) {
    iter->second->onHandleEvent(event_in.at(iter->first));
    std::cout << "log---fsm id: " << iter->first << " event in : " << event_in.at(iter->first) << std::endl;
  }
  event_in.clear();
  return true;
}

bool FSMManager::postEvent() {
  for(auto iter = m_all_FSMMap.begin(); iter != m_all_FSMMap.end(); iter++) {
    event_out[iter->first] = iter->second->post_event;
    std::cout << "log---fsm id: " << iter->first << " post event: " << iter->second->post_event << std::endl;
  }
  return true;
}
