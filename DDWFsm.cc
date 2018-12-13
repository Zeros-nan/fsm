#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "DDWFsm.h"


void initStatus::onStateEnter() {
  std::cout << "FSM" << "init state enter" << std::endl;
}

void initStatus::onStateExit() {
  std::cout << "FSM" << "init state exit" << std::endl;
}

void initStatus::onStateAction() {
  std::cout << "FSM" << " init action" << std::endl;
  if(fsm->checkEnableOrNot() == true || fsm->warning_or_normal == true)
    fsm->tranState(DDWState::s_normal);
  if(fsm->warning_or_normal == false && fsm->checkEnableOrNot() == false)
    fsm->tranState(DDWState::s_warnning);
}

void normalStatus::onStateEnter() {
  std::cout << "normal state enter" << std::endl;
}

void normalStatus::onStateAction() {
  std::cout << "normal state action" << std::endl;
  if(fsm->getPreStareId() == DDWState::s_output) fsm->post_event = true;
  if(fsm->warning_or_normal == true) {
    fsm->tranState(DDWState::s_output);
  }
  if(fsm->warning_or_normal == false && fsm->checkEnableOrNot() == false) {
    fsm->tranState(DDWState::s_warnning);
  }
}

void normalStatus::onStateExit() {
  std::cout << "normal state exit" << std::endl;
}

void waringStatus::onStateEnter() {
  std::cout << "warning state enter" << std::endl;
  time_manager->addTimer(DDWWarnTime::startTime, fsm->m_fsm_time);
  time_manager->addTimer(DDWWarnTime::endTime, fsm->m_fsm_time);
  time_manager->startTime(DDWWarnTime::endTime);
}

void waringStatus::onStateExit() {
  std::cout << "warning state exit" << std::endl;
  time_manager->deleteTimer(DDWWarnTime::startTime);
  time_manager->deleteTimer(DDWWarnTime::endTime);
  time_manager->stopTime(DDWWarnTime::endTime);
}

void waringStatus::onStateAction() {
  std::cout << "warning state action" << std::endl;
  time_manager->update(fsm->m_fsm_time);
  if(fsm->warning_or_normal == true){
    fsm->tranState(DDWState::s_normal);
  }
  if(fsm->warning_or_normal == false) {
    if(time_manager->findTimerById(DDWWarnTime::endTime)->get()
        - time_manager->findTimerById(DDWWarnTime::startTime)->get() >= durationTimems(3000)) {
      fsm->tranState(DDWState::s_output);
    }
  }
}

void outputStatus::onStateEnter() {
  std::cout << "output state enter" << std::endl;
  onStateAction();
}

void outputStatus::onStateExit() {
  std::cout << "output state enter" << std::endl;
}

void outputStatus::onStateAction() {
  std::cout << "output state action" << std::endl;
  if(fsm->getPreStareId() == DDWState::s_normal) {
    fsm->post_event = true;
  }
  if(fsm->getPreStareId() == DDWState::s_warnning) {
    fsm->post_event = false;
  }
  fsm->tranState(DDWState::s_normal);
}

DDWFSM::DDWFSM() : FsmSimple("ddw_fsm"),
                   init_state_(nullptr),
                   normal_state_(nullptr),
                   warnning_state_(nullptr),
                   output_state_(nullptr) {}

bool DDWFSM::init() {
  init_state_ = new initStatus(this);
  normal_state_ = new normalStatus(this);
  warnning_state_ = new waringStatus(this);
  output_state_ = new outputStatus(this);
  registerState(DDWState::s_init, init_state_);
  registerState(DDWState::s_normal, normal_state_);
  registerState(DDWState::s_warnning, warnning_state_);
  registerState(DDWState::s_output, output_state_);
  if(!setInitState(DDWState::s_init))
    return false;
  m_fsm_inited = true;
  return true;
}

bool DDWFSM::Fini() {
  if(!m_init_state)
    return false;
  if(init_state_)
    delete init_state_;
  if(normal_state_)
    delete normal_state_;
  if(warnning_state_)
    delete warnning_state_;
  if(output_state_)
    delete output_state_;
  return true;
}

bool DDWFSM::start() {
  if(!init()) {
    std::cout << "init failed" << std::endl;
    return false;
  }
  if(!checkExist(DDWState::s_init)) return false;
  m_all_stateUMap.at(m_init_state)->onStateEnter();
  m_active_stId = m_init_state;
  m_active_state = m_all_stateUMap.at(m_init_state);
  m_fsm_running = true;
  return true;
}

bool DDWFSM::stop() {
  if(!m_fsm_running) return false;
  if(!tranState(m_init_state)) return false;
  m_fsm_running = false;
  return true;
}

bool DDWFSM::suspend() {
  if(!m_fsm_running) return false;
  m_fsm_running = false;
  return true;
}

bool DDWFSM::resume() {
  if(m_fsm_running) return false;
  m_fsm_running = true;
  return true;
}

bool DDWFSM::checkEnableOrNot() {
  return false;
}

void DDWFSM::onHandleEvent(const bool event) {
  if(!m_fsm_running) {
    std::cout << "fsm is not running" << std::endl;
    return;
  }
  warning_or_normal = event;
}

bool DDWFSM::updateTime(const timePoint time_) {
  std::cout << "time: " << time_.time_since_epoch().count() << std::endl;
  m_fsm_time = time_;
  return true;
}

//每帧都被执行
bool DDWFSM::onTick() {
  checkEnableOrNot();
  getActiveState()->onStateAction();
  return true;
}
