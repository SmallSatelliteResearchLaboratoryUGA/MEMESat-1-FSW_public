// ======================================================================
// \title  Scheduler.cpp
// \author g83r
// \brief  cpp file for Scheduler component implementation class
// ======================================================================

#include "Components/Scheduler/Scheduler.hpp"

/*
 * TODO: Needs testing 
 */
std::string formatDuration(std::chrono::system_clock::duration duration)
{
  using namespace std::chrono;
  
  auto hours_ = duration_cast<hours>(duration).count();
  duration -= hours(hours_);

  auto minutes_ = duration_cast<minutes>(duration).count();
  duration -= minutes(minutes_);

  auto seconds_ = duration_cast<seconds>(duration).count();
  duration -= seconds(seconds_);

  auto milliseconds_ = duration_cast<milliseconds>(duration).count();
  duration -= milliseconds(milliseconds_);

  std::ostringstream oss;
  if (hours_ > 0)
    oss << hours_ << " hours_ ";
  if (minutes_ > 0) 
    oss << minutes_ << " minutes_ ";
  if (seconds_ > 0)
    oss << seconds_ << " seconds_ ";
  if (milliseconds_ > 0 || hours_ == 0 && minutes_ == 0 && seconds_ == 0)
    oss << milliseconds_ << " ms";

  return oss.str();
} // formatDuration

namespace SchedulerModule {

  /*
   * 'Cron' is a template class. When included in a cpp file,
    * the compiler can figure out the template arguments for the
    * object. However, if it is included in an hpp file, the
    * compiler is unaware of what the template arguments will be
    * for all implementations of the object. Thus, the Cron object
    * must exist in this cpp file and any actions necessary for
    * testing need to be defined here.
    * 
    */
  // can figure above out after run
  libcron::Cron<libcron::LocalClock, libcron::Locker> cron;

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  Scheduler ::
    Scheduler(const char* const compName) :
      SchedulerComponentBase(compName)
  {

  } // Scheduler

  void Scheduler ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    SchedulerComponentBase::init(queueDepth, instance);
  } // init

  Scheduler ::
    ~Scheduler()
  {

  } // ~Scheduler

  static char* schedule_parser(const char* input) 
  {
    // Regex pattern for cron scheduling according to libcron GitHub page
    const std::string cron_pattern =
        R"((^((((\d+,)+\d+|(\d+(\/|-|#)\d+)|\d+L?|\*(\/\d+)?|L(-\d+)?|\?|[A-Z]{3}(-[A-Z]{3})?) ?){5,7})$)|"
        R"((@(annually|yearly|monthly|weekly|daily|hourly|reboot)))|"
        R"((@every (\d+(ns|us|µs|ms|s|m|h))+)))";
    // Compile the regex
    std::regex pattern(cron_pattern);
    // Convert the input char* to a std::string for regex matching
    std::string input_str(input);
    // Match input with the cron pattern
    if (std::regex_match(input_str, pattern)) 
    {
      return (char*)"Valid schedule";
    } // if 
    else 
    {
      return NULL;
    } // if
  } // schedule_parser

  // --- 
  // Custom Functions 
  // --- 

  void schedule_list_to_file()
  {
    // Do we need to handle potential queuing on the mutex? 
    std::vector<std::tuple<std::string, std::chrono::system_clock::duration>> status;
    cron.get_time_until_expiry_for_tasks(status);
    std::ofstream file_of_schedules;
    // MemberTypeFailure exception thrown on fail 
    // Check failbit 
    // Opens the file 
    /*
     * TODO: Handle exception, check failbit, make 'schedules.txt' mutable
     */
    file_of_schedules.open("schedule_list.txt", std::ios::trunc);
    // If it is open, loop through the tasks on the cron object, 
    // get the status (name, time unil expiry) and shove into file 
    if (file_of_schedules.is_open()) {
      for (auto& [task, duration] : status) {
        file_of_schedules << task << " " << duration.count() << std::endl;
      }
      file_of_schedules.close();
    }
    /* 
     * TODO: Log if file was not opened. 
     */
  } // schedule_list_to_file

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Scheduler ::
    getSchedule_handler(
        FwIndexType portNum,
        const Fw::StringBase& name,
        const Fw::StringBase& schedule,
        const SchedulerModule::ScheduleOp& action
    )
  {
    const char* name_str = name.toChar();
    const char* schedule_str = schedule.toChar();
    // Make sure that the input port is not greater than the actual number of ports
    NATIVE_INT_TYPE actual_number_of_ports = this->getNum_getSchedule_InputPorts();
    if (actual_number_of_ports < portNum) {
        this->log_WARNING_LO_SCH_PortNumGetScheduleWarn(portNum);
        return;
    } // if
    // add character check, ensure correctly formatted
    if (name_str == NULL || name_str[0] == '\0') {
      this->log_WARNING_LO_SCH_NameStringEmptyWarn(portNum);
      return;
    } // if
    // add schedule parsing, use event SCHEDULE_INCORRECT(schedule_str, portNum, name_str, status) 
    // only parse if action != STOP 
    bool isTask = false; 
    std::vector<std::tuple<std::string, std::chrono::system_clock::duration>> task_expiry_times;
    cron.get_time_until_expiry_for_tasks(task_expiry_times);
    for (auto& [task, duration] : task_expiry_times) {
      if (strcmp(task.c_str(), name_str) == 0) {
        isTask = true;
        //break;
      } // if
    } // for
    SchedulerModule::ScheduleStatus currentStatus = isTask ? ScheduleStatus::RUNNING : ScheduleStatus::STOPPED;
    // Make a separate event for before run vs after run 
    this->log_ACTIVITY_LO_SCH_TaskRunning(Fw::String(name_str), currentStatus);
    try { 
      switch(action.e)
      {
        case SchedulerModule::ScheduleOp::START:
          if (isTask) 
          {
            cron.remove_schedule(name_str);
            currentStatus = SchedulerModule::ScheduleStatus::STOPPED;
            break;
          }
          currentStatus = SchedulerModule::ScheduleStatus::RUNNING;
          cron.add_schedule(name_str, schedule_str, [this, name_str](const libcron::TaskInformation& taskInfo) {
            this->runSchedule_out(0, SchedulerModule::ScheduleStatus::RUNNING);
          });
          break;
        case SchedulerModule::ScheduleOp::STOP:
          cron.remove_schedule(name_str);
          currentStatus = SchedulerModule::ScheduleStatus::STOPPED;
          break;
        default: 
          currentStatus = SchedulerModule::ScheduleStatus::FAILED;
          break;
      } // switch
    } // try
    catch(...) {
      currentStatus = SchedulerModule::ScheduleStatus::FAILED;
    } // catch
    this->log_ACTIVITY_LO_SCH_TaskRunning(Fw::String(name_str), currentStatus);
    // Telemetry Specific 
    if (portNum == 0) 
      this->tlmWrite_SCH_FileRecyclerPortStatus(currentStatus);
    else if (portNum == 1)
      this->tlmWrite_SCH_TlmChanPortStatus(currentStatus);
  } // getSchedule_handler

  void Scheduler ::
    tick_handler(
        FwIndexType portNum,
        U32 context
    )
  {
    // Ensure that this doesn't throw anything
    cron.tick();
  } // tick_handler

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void Scheduler ::
    SCH_GetScheduleList_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        const Fw::CmdStringArg& destFileName
    )
  {
    const char* destination_file_name = destFileName.toChar();
    schedule_list_to_file();
    const char* schedule_file_name = "schedule_list.txt";
    this->downlinkCurrentSchedules_out(0, Fw::String(schedule_file_name), Fw::String(destination_file_name), 0, 0);
    this->log_DIAGNOSTIC_SCH_ScheduleDownlinked(Fw::String(destination_file_name));
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // SCH_GetScheduleList_cmdHandler

  void Scheduler ::
    SCH_StopSchedule_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        const Fw::CmdStringArg& name
    )
  {
    cron.remove_schedule(name.toChar());
    // Check if stopped 
    this->log_ACTIVITY_LO_SCH_TaskRunning(Fw::String(name), ScheduleStatus::STOPPED);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // SCH_StopSchedule_cmdHandler

  void Scheduler ::
    SCH_CreateSchedule_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq,
      const Fw::CmdStringArg& name,
      const Fw::CmdStringArg& schedule,
      const Fw::CmdStringArg& capture_param, 
      const Fw::CmdStringArg& lambda_param, 
      const Fw::CmdStringArg& lambda_fn
    )
  {
    // Check if schedule is valid
    this->log_WARNING_LO_SCH_CommandSent(Fw::String(name), Fw::String(schedule)); 
    // Check if schedule is valid
    if (schedule_parser(schedule.toChar()) == NULL) 
    {
      this->log_WARNING_LO_SCH_ScheduleIncorrectWarn(Fw::String(schedule.toChar()), 1, Fw::String(name.toChar()), ScheduleStatus::FAILED);
      return;
    } // if
    else 
    {
      this->log_WARNING_LO_SCH_ScheduleProcessed(Fw::String(schedule.toChar()), Fw::String(name.toChar()));
    } // if
    this->log_WARNING_LO_SCH_ScheduleIncorrectWarn(Fw::String(schedule.toChar()), 2, Fw::String(name.toChar()), ScheduleStatus::FAILED);
  } // SCH_CreateSchedule_cmdHandler
} // namespace SchedulerModule
