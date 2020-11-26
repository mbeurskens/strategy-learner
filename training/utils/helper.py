""" helper.py
    Helper function for strategy learner.
"""

import functools


def rgetattr(obj, attr, *args):
    """ Split field string to get nested fields. """

    def _getattr(obj, attr):
        return getattr(obj, attr, *args)

    return functools.reduce(_getattr, [obj] + attr.split('.'))


def check_type_in_list(array, datatype):
    """Check if first entry of a list is of a certain data type."""
    if not array:
        raise ImportError('No data found!')
    elif not isinstance(array[0], datatype):
        raise ImportError('Incorrect data format!')
