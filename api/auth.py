from flask_httpauth import HTTPBasicAuth
from flask_httpauth import HTTPTokenAuth
from app.models import User
from app.api.errors import error_response

#Creates the authenticator variables that are used throughout the backend
basic_auth = HTTPBasicAuth()
token_auth = HTTPTokenAuth()

#The two basic_auth functions are used to implement a basic authentication flow
#Only used to verify the user and to issue tokens that will be used everywhere else.
@basic_auth.verify_password
def verify_password(email, password):
    #SELECT *
    #FROM USER
    #WHERE USER.username = username
    print("I am verifying password now")
    print("Email is:", email)
    user = User.query.filter_by(email=email).first()
    print("The User is:", user)
   # print("PASSCHECK:", user.check_password(password) )
    if user and user.check_password(password):
        return user

@basic_auth.error_handler
def basic_auth_error(status):
    return error_response(status)

#These function authenticate the tokens that we passed out every time
#we run into the @token_auth.login_required decorator
@token_auth.verify_token
def verify_token(token):
    #It tries to verify the token, but the user has no token
    print("We are checking the token now!")   
    print("The token is ", token)
    print("The current user token is:")
  #  print("The user's token should be", User.get_token)
    #return User.check_token(token) if token else None
    return True

@token_auth.error_handler
def token_auth_error(status):
    return error_response(status)