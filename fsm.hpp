#include <unordered_map>
#include <map>
#include <set>
#include <chrono>
#include <list>
#include <string>
#include <iostream>

typedef std::chrono::steady_clock::time_point timePoint;
typedef std::chrono::milliseconds durationTimems;

/**
* @brief: Timer
*/
class Timer {
  public:
    Timer(timePoint timeVal) : m_time_val(timeVal){}
    bool update(const timePoint time_) {
      m_time_val = time_;
      return true;
    }

    timePoint get() {
      return m_time_val;
    }
    ~Timer(){}
  private:
    timePoint m_time_val;
};

/**
* @brief: Time Manager
*/
class TimeManger {
  public:
    TimeManger(std::string name_) : m_name(name_) {}
    ~TimeManger() {
      if(!m_timeMap.empty()) {
        for(auto t : m_timeMap) {
          delete t.second;
        }
        m_timeMap.clear();
      }
    }
    bool startTime(const int timerId) {
      auto ret = running_timer.insert(timerId);
      if(!ret.second) return false;
      return true;
    }
    bool stopTime(const int timerId) {
      auto ret = running_timer.erase(timerId);
      if(!ret) return false;
      return true;
    }
    bool addTimer(int timerId, Timer* time_) {
      auto ret = m_timeMap.insert(std::pair<int, Timer*>(timerId, time_));
      if(!ret.second) return false;
      return true;
    }
    bool addTimer(int timerId, const timePoint time_) {
      auto ret = m_timeMap.insert(std::pair<int, Timer*>(timerId, new Timer(time_)));
      if(!ret.second) return false;
      return true;
    }

    bool deleteTimer(int timerId) {
      delete m_timeMap.at(timerId);
      auto ret = m_timeMap.erase(timerId);
      if(!ret) {
        std::cout << "not found" << std::endl;
        return false;
      }
      return true;
    }

    bool update(const timePoint time_) {
      for(auto t : running_timer) {
        m_timeMap.at(t)->update(time_);
      }
      return true;
    }

    Timer* findTimerById(const int timerId) {
      if(m_timeMap.find(timerId) == m_timeMap.end()) {
        std::cout << "currnet is not exist" << std::endl;
        return nullptr;
      }
      return m_timeMap.at(timerId);
    }
  private:
    std::map<int, Timer*> m_timeMap;
    std::set<int> running_timer;
    std::string m_name;
};

class State;
/**
* @brief: FsmSimple, Fsm imp
*/
class FsmSimple {
  public:

    timePoint m_fsm_time;
    bool post_event;
    bool warning_or_normal;

    FsmSimple(std::string fsm_name_);
    virtual ~FsmSimple() {}
    virtual bool init() = 0;
    virtual bool Fini() = 0;
    virtual bool suspend() = 0; //停止更新事件
    virtual bool resume() = 0;   //重新更新事件
    virtual bool start() = 0;//置到init状态，并将开始更新事件
    virtual bool stop() = 0;  //置到init状态,并停止更新事件
    virtual bool onTick() = 0;
    virtual bool checkEnableOrNot() = 0;
    virtual void onHandleEvent(const bool event) = 0;
    // const int getFsmId();
    bool setInitState(const int stateId);
    bool checkExist(const int stateId);
    //true 表示当前warning被允许  false 表示当前状态不被允许
    bool tranState(const int stateId);
    bool registerState(const int stateId, State *p_state);
    const int getPreStareId();
    State* getActiveState();
    State* findStateById(const int state_id);
    virtual bool updateTime(const timePoint time_) = 0;

  protected:
    std::string m_fsm_name;
    bool m_fsm_inited;
    int m_init_state;
    int m_active_stId;
    int m_preAct_stId;
    bool m_fsm_running;
    State* m_active_state;   //active state

    // id 0 state must be init state
    std::unordered_map<int, State *> m_all_stateUMap;
};

/**
* @brief: State, the base of all state
*/
class State {
  public:
    State(FsmSimple* fsm) : fsm(fsm) {}
    virtual ~State(){}

    virtual void onStateEnter() = 0;
    virtual void onStateAction() = 0;
    virtual void onStateExit() = 0;

  protected:
    FsmSimple* fsm;
};

/**
* @brief: FsmFramework
*/
class FSMManager {
  public:

    std::unordered_map<int, bool> event_in;
    std::unordered_map<int, bool> event_out;

    timePoint m_time;

    FSMManager();
    ~FSMManager(){}

    bool startFSM(const int FSMId);
    bool stopFSM(const int FSMId);
    bool suspendFSM(const int FSMId);
    bool resumeFSM(const int FSMId);

    bool postEvent();                         //将事件抛出
    bool dispatchEvent();                     //将事件分发
    bool registerFSM(const int FSMId, FsmSimple* FSM);
    bool unregisterFSM(const int FSMId);
    bool checkExist(const int FSMId);
    bool onTick();

private:
    std::unordered_map<int, FsmSimple*> m_all_FSMMap;
    std::set<int> m_running_FSMSet;
};

