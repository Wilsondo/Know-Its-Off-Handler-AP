from flask import Blueprint

bp = Blueprint('api', __name__)

from handler.api import user, errors, device