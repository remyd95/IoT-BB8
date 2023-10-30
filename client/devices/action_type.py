from enum import Enum


class ActionType(Enum):
    """
    Enum class for the different actions that can be performed by the ball.
    Caution: These values should match the ones programmed in the ball.
    """
    IDLE = 0
    MOVETO = 1
    FORWARD = 2
    BACKWARD = 3
    STOP = 4
    INIT = 5
    REBOOT = 6
    TURN_LEFT = 7
    TURN_RIGHT = 8
    UNDEFINED = -1


def get_action_from_value(value):
    """
    Returns the action type corresponding to the given value.
    :param value: The value of the action.
    :return: The action type.
    """
    for action in ActionType:
        if action.value == value:
            return action
    return ActionType.UNDEFINED
