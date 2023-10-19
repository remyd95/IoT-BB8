from enum import Enum


class ActionType(Enum):
    IDLE = 0
    MOVETO = 1
    FORWARD = 2
    BACKWARD = 3
    STOP = 4
    INIT = 5
    REBOOT = 6
    FIND_AVAILABLE = 7
    UNDEFINED = -1


def get_action_from_value(value):
    for action in ActionType:
        if action.value == value:
            return action
    return ActionType.UNDEFINED
