#include <string>
#include "fsm.hpp"


enum DDWState {
  s_init = 0,
  s_normal = 1,
  s_warnning = 2,
  s_output = 3
};

/**
* @brief: DDW FSM
*/
class DDWFSM : public FsmSimple {
  public:
    DDWFSM();
    virtual ~DDWFSM() { Fini();}


    bool init() override;
    bool Fini() override;
    bool suspend() override;
    bool resume() override;
    bool start() override;
    bool stop() override;

    bool onTick() override;
    //true 为nornal， false 为warning
    void onHandleEvent(const bool event) override;
    //true 表示当前warning被允许  false 表示当前状态不被允许
    bool checkEnableOrNot() override;
    bool updateTime(const timePoint time_) override;

  private:
    State* init_state_;
    State* normal_state_;
    State* warnning_state_;
    State* output_state_;

};

/**
* @brief: Init Status
*/
class initStatus : public State {
  public:
    initStatus(DDWFSM* fsm_) : State(fsm_){}
    virtual ~initStatus() {}
    void onStateEnter() override;
    void onStateExit() override;
    void onStateAction() override;
};

/**
* @brief: Normal Status
*/
class normalStatus : public State {
  public:
    normalStatus(DDWFSM* fsm_) : State(fsm_) {}
    virtual ~normalStatus() {}
    void onStateEnter() override;
    void onStateExit() override;
    void onStateAction() override;
};

enum DDWWarnTime {
  startTime = 0,
  endTime = 1
};
/**
* @brief: Warning Status
*/
class waringStatus : public State {
  public:
    waringStatus(DDWFSM* fsm_) : State(fsm_) {
      time_manager = new TimeManger("warnning");
    }
    virtual ~waringStatus() {
      if(time_manager)
        delete time_manager;
    }
    void onStateEnter() override;
    void onStateExit() override;
    void onStateAction() override;

  private:
    TimeManger* time_manager;
};


/**
* @brief: Output Status
*/
class outputStatus : public State {
  public:
    outputStatus(DDWFSM* fsm_) : State(fsm_) {}
    virtual ~outputStatus() {}
    void onStateEnter() override;
    void onStateExit() override;
    void onStateAction() override;
};

