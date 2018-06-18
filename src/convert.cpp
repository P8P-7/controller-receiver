#include <utility>

#include "convert.h"


std::vector<commands::ServoCommand_Motor> front = {commands::ServoCommand_Motor_LEFT_FRONT,
                                                  commands::ServoCommand_Motor_RIGHT_FRONT};

std::vector<commands::ServoCommand_Motor> back = {commands::ServoCommand_Motor_LEFT_BACK,
                                                 commands::ServoCommand_Motor_RIGHT_BACK};


MessageCarrier dualJoystickToMove(CONTROL control, int value) {
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new commands::MoveCommand);
    commands::MotorCommand_Gear gear;
    std::deque<commands::MotorCommand_Motor> motors;

    // Apply sensitivity
    value = value * getConfig(SENSITIVITY) / 255 / 2;

    // Select gear
    if (value < 0) {
        gear = commands::MotorCommand_Gear_FORWARD;
    } else if (value > 0) {
        gear = commands::MotorCommand_Gear_BACKWARD;
    } else {
        gear = commands::MotorCommand_Gear_LOCK;
    }

    // Select motors
    if (control == JSLY) {
        motors.emplace_back(commands::MotorCommand_Motor_LEFT_FRONT);
        motors.emplace_back(commands::MotorCommand_Motor_LEFT_BACK);
    } else if (control == JSRY) {
        motors.emplace_back(commands::MotorCommand_Motor_RIGHT_FRONT);
        motors.emplace_back(commands::MotorCommand_Motor_RIGHT_BACK);
    }

    // Add motor commands to single move command
    for (commands::MotorCommand_Motor motor : motors) {
        commands::MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(abs(value));
    }

    // Put move command in a message
    commandMessage->set_allocated_movecommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    BOOST_LOG_TRIVIAL(debug) << "Axis \"" << control << "\" moved to \"" << value << "\".";

    return message;
}

MessageCarrier buttonToFrontWing(CONTROL control, int value) {
    int speed = 0;
    if (value == 1) {
        speed = getConfig(SENSITIVITY) * 4;
    }

    if (value == 0) {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }

    switch (control) {
        case BTN1:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_FRONT, commands::ServoCommand_Direction_DOWN,
                                     speed);
        case BTN2:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_FRONT, commands::ServoCommand_Direction_UP,
                                     speed);
        case BTN3:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_FRONT, commands::ServoCommand_Direction_UP,
                                     speed);
        case BTN4:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_FRONT, commands::ServoCommand_Direction_DOWN,
                                     speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier buttonToBackWing(CONTROL control, int value) {
    int speed = 0;
    if (value == 1) {
        speed = getConfig(SENSITIVITY) * 4;
    }

    if (value == 0) {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }

    switch (control) {
        case BTN1:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_BACK, commands::ServoCommand_Direction_UP,
                                     speed);
        case BTN2:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_BACK, commands::ServoCommand_Direction_DOWN,
                                     speed);
        case BTN3:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_BACK, commands::ServoCommand_Direction_DOWN,
                                     speed);
        case BTN4:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_BACK, commands::ServoCommand_Direction_UP,
                                     speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier buttonToAllWing(CONTROL control, int value) {
    int speed = 0;
    if (value == 1) {
        speed = getConfig(SENSITIVITY) * 4;
    }

    if (value == 0) {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }

    switch (control) {
        case BTN1:
            return toMoveWingMessage(front, commands::ServoCommand_Direction_UP, speed);
        case BTN2:
            return toMoveWingMessage(front, commands::ServoCommand_Direction_DOWN, speed);
        case BTN3:
            return toMoveWingMessage(back, commands::ServoCommand_Direction_DOWN, speed);
        case BTN4:
            return toMoveWingMessage(back, commands::ServoCommand_Direction_UP, speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier inputToCommand(CommandMessage::CommandCase commandCase){
    MessageCarrier message;
    CommandMessage *command = new CommandMessage;

    switch (commandCase){
        case CommandMessage::kDanceCommand:
            command->set_allocated_dancecommand(new commands::DanceCommand);
            BOOST_LOG_TRIVIAL(info) << "Received DanceCommand from controller";
            break;
        case CommandMessage::kEnterCommand:
            command->set_allocated_entercommand(new commands::EnterCommand);
            BOOST_LOG_TRIVIAL(info) << "Received EnterCommand from controller";
            break;
        case CommandMessage::kInterruptCommandCommand:
            command->set_allocated_interruptcommandcommand(new commands::InterruptCommandCommand);
            BOOST_LOG_TRIVIAL(info) << "Received InterruptCommandCommand from controller";
            break;
        case CommandMessage::kInvalidateAllCommand:
            command->set_allocated_invalidateallcommand(new commands::InvalidateAllCommand);
            BOOST_LOG_TRIVIAL(info) << "Received InvalidateAllCommand from controller";
            break;
        case CommandMessage::kLineDanceCommand:
            command->set_allocated_linedancecommand(new commands::LineDanceCommand);
            BOOST_LOG_TRIVIAL(info) << "Received LineDanceCommand from controller";
            break;
        case CommandMessage::kMoveCommand:
            command->set_allocated_movecommand(new commands::MoveCommand);
            BOOST_LOG_TRIVIAL(info) << "Received MoveCommand from controller";
            break;
        case CommandMessage::kMoveWingCommand:
            command->set_allocated_movewingcommand(new commands::MoveWingCommand);
            BOOST_LOG_TRIVIAL(info) << "Received MoveWingCommand from controller";
            break;
        case CommandMessage::kObstacleCourseCommand:
            command->set_allocated_obstaclecoursecommand(new commands::ObstacleCourseCommand);
            BOOST_LOG_TRIVIAL(info) << "Received ObstacleCourseCommand from controller";
            break;
        case CommandMessage::kShutdownCommand:
            command->set_allocated_shutdowncommand(new commands::ShutdownCommand);
            BOOST_LOG_TRIVIAL(info) << "Received ShutdownCommand from controller";
            break;
        case CommandMessage::kSynchronizeCommandsCommand:
            command->set_allocated_synchronizecommandscommand(new commands::SynchronizeCommandsCommand);
            BOOST_LOG_TRIVIAL(info) << "Received SynchronizeCommandsCommand from controller";
            break;
        case CommandMessage::kTransportRebuildCommand:
            command->set_allocated_transportrebuildcommand(new commands::TransportRebuildCommand);
            BOOST_LOG_TRIVIAL(info) << "Received TransportRebuildCommand from controller";
            break;
        case CommandMessage::kWunderhornCommand:
            command->set_allocated_wunderhorncommand(new commands::WunderhornCommand);
            BOOST_LOG_TRIVIAL(info) << "Received WunderhornCommand from controller";
            break;
        default:
            BOOST_LOG_TRIVIAL(info) << "Received unknown from controller";
            break;
    }

    message.set_allocated_commandmessage(command);

    return message;
}

MessageCarrier
convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL, int)>> functionMap) {
    if (functionMap[control] != nullptr) {
        return functionMap[control](control, value);
    }
    return MessageCarrier();
}

MessageCarrier
toMoveWingMessage(commands::ServoCommand_Motor wing, commands::ServoCommand_Direction direction, int speed) {
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new commands::MoveWingCommand);

    commands::ServoCommand *wingCommand = moveCommand->add_commands();
    wingCommand->set_motor(wing);
    wingCommand->set_speed(speed);
    wingCommand->set_direction(direction);

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    return message;
}

MessageCarrier
toMoveWingMessage(std::vector<commands::ServoCommand_Motor> wings, commands::ServoCommand_Direction direction,
                  int speed) {
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new commands::MoveWingCommand);

    for (commands::ServoCommand_Motor wing : wings) {
        commands::ServoCommand *wingCommand = moveCommand->add_commands();
        wingCommand->set_motor(wing);
        wingCommand->set_speed(speed);
        if(wing == commands::ServoCommand_Motor_LEFT_FRONT || wing == commands::ServoCommand_Motor_LEFT_BACK){
            if(direction == commands::ServoCommand_Direction_UP){
                wingCommand->set_direction(commands::ServoCommand_Direction_DOWN);
            }
            else{
                wingCommand->set_direction(commands::ServoCommand_Direction_UP);
            }
        }
        else{
            wingCommand->set_direction(direction);
        }
    }

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    return message;
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(std::stoi(string));
}

CommandMessage::CommandCase stringToCommandCase (std::string string){
    return static_cast<CommandMessage::CommandCase >(std::stoi(string));
}
