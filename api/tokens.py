from flask import jsonify
from app import db
from app.api import bp
from app.api.auth import basic_auth
from app.api.auth import token_auth


#This function returns the basic login token to the user for identification
@bp.route('/tokens', methods=['POST'])
@basic_auth.login_required
def get_token():
   # print("THe current user is", current_user.get_id())
    print("we are getting a token now")
    token = basic_auth.current_user().get_token()
    print("The token is ", token)
    print("ALL DONE")
    db.session.commit()
    return jsonify({'token': token})

#This function returns the main token to the user for identification
@bp.route('/tokens', methods=['DELETE'])
@token_auth.login_required
def revoke_token():
    token_auth.current_user().revoke_token()
    db.session.commit()
    return '', 204
