#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "DDWFsm.h"

enum FSM {
  DDW = 0
};

int main(int argc, char *argv[])
{
  DDWFSM* ddw = new DDWFSM();
  FSMManager* manager = new FSMManager();
  bool event;

  manager->registerFSM(FSM::DDW, ddw);
  manager->startFSM(FSM::DDW);

  while(std::cin >> event) {
    manager->m_time = std::chrono::steady_clock::now();
    manager->event_in.insert(std::make_pair(FSM::DDW, event));
    manager->onTick();
    manager->event_in.clear();
    std::cout << "--------------------------------------" << std::endl;
  }

  delete ddw;
  delete manager;
  return 0;
}
