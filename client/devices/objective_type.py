from enum import Enum


class ObjectiveType(Enum):
    """
    Enum class for the different objectives that the ball can have.
    Caution: These values should match the ones programmed in the ball.
    """
    NONE = 0
    MOVETO = 1
    FORWARD = 2
    BACKWARD = 3
    STOP = 4
    TURN_LEFT = 5
    TURN_RIGHT = 6
    INIT = 7
    REBOOT = 8
    UNDEFINED = -1


def get_objective_from_value(value):
    """
    Returns the objective type corresponding to the given value.
    :param value: The value of the objective.
    :return: The objective type.
    """
    for objective in ObjectiveType:
        if objective.value == value:
            return objective
    return ObjectiveType.UNDEFINED
