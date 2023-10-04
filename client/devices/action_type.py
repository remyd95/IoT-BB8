from enum import Enum


class ActionType(Enum):
    MOVETO = 0
    FORWARD = 1
    BACKWARD = 2
    STOP = 3


def get_action_from_value(value):
    for action in ActionType:
        if action.value == value:
            return action
    return -1
