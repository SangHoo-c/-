from flask import Flask, jsonify, render_template, request
from flask_sqlalchemy import SQLAlchemy
import random

app = Flask(__name__)

##Connect to Database
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///cafes.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)


##Cafe TABLE Configuration
# db.Column : Table 의 column 을 만든다,
class Cafe(db.Model):
    # __init__ method 는 SQLAlchemy 가 자동 생성

    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(250), unique=True, nullable=False)
    map_url = db.Column(db.String(500), nullable=False)
    img_url = db.Column(db.String(500), nullable=False)
    location = db.Column(db.String(250), nullable=False)
    seats = db.Column(db.String(250), nullable=False)
    has_toilet = db.Column(db.Boolean, nullable=False)
    has_wifi = db.Column(db.Boolean, nullable=False)
    has_sockets = db.Column(db.Boolean, nullable=False)
    can_take_calls = db.Column(db.Boolean, nullable=False)
    coffee_price = db.Column(db.String(250), nullable=True)

    # serialising our database row Object to JSON
    # converting it to a JSON structure.
    def to_dict(self):
        cafe_dict = {}
        for column in self.__table__.columns:
            cafe_dict[column.name] = getattr(self, column.name)
        return cafe_dict

    def to_dict_2(self):
        return {column.name: getattr(self, column.name) for column in self.__table__.columns}


@app.route("/")  # decorator 패턴, URL 연결에 사용
def home():
    return render_template("index.html")


## HTTP GET - Read Record
@app.route("/random")
def random_get_request():
    cafes = db.session.query(Cafe).all()
    random_cafe = random.choice(cafes)

    # dict type 의 자료를 반환하는 Cafe class 의 method 를 정의한다.
    """
    return jsonify(cafe={
        "id": random_cafe.id,
        "name": random_cafe.name,
        "map_url": random_cafe.map_url,
        "img_url": random_cafe.img_url,
        "location": random_cafe.location,
        "seats": random_cafe.seats,
        "has_toilet": random_cafe.has_toilet,
        "has_wifi": random_cafe.has_wifi,
        "has_sockets": random_cafe.has_sockets,
        "can_take_calls": random_cafe.can_take_calls,
        "coffee_price": random_cafe.coffee_price,
    })
    """
    return jsonify(cafe=random_cafe.to_dict())


@app.route("/all")
def all_get_request():
    cafes = db.session.query(Cafe).all()
    """
    all_list = []
    for cafe in cafes:
        all_list.append(cafe.to_dict())
    return jsonify({"cafes": all_list})
    """
    return jsonify(cafes=[cafe.to_dict() for cafe in cafes])


@app.route("/search")
def search_get_request():
    # flask 인자를 받는 방법
    # url : http://127.0.0.1:5000/search?loc=Peckham
    # args.get 을 통해 입력받은 URL 에서 인자를 받아온다.
    query_location = request.args.get("loc")
    cafes = db.session.query(Cafe).filter_by(location=query_location).all()
    # cafes = db.session.query.filter_by(location=loc).all()
    # return jsonify(cafe=[cafe.to_dict() for cafe in cafes])
    if cafes:
        return jsonify(cafes=[cafe.to_dict() for cafe in cafes])
    else:
        return jsonify(error={"Not Found": "Sorry, we don't have a cafe at that location."})


## HTTP POST - Create Record
@app.route('/add', methods=["POST"])
def add_post_request():
    new_cafe = Cafe(
        name=request.form.get("name"),
        map_url=request.form.get("map_url"),
        img_url=request.form.get("img_url"),
        location=request.form.get("loc"),
        has_sockets=bool(request.form.get("sockets")),
        has_toilet=bool(request.form.get("toilet")),
        has_wifi=bool(request.form.get("wifi")),
        can_take_calls=bool(request.form.get("calls")),
        seats=request.form.get("seats"),
        coffee_price=request.form.get("coffee_price"),
    )
    db.session.add(new_cafe)
    db.session.commit()
    return jsonify(response={"success": "Successfully added the new cafe."})


## HTTP PUT/PATCH - Update Record
# variable rules.
# 원하는 cafe-id 를 어떻게 받는지!
# "PATCH" 오류
# This is because your browser can only handle GET and POST requests.
# More precisely, the HTML5 specification only supports GET and POST.
@app.route("/update-price/<int:cafe_id>", methods=["POST"])
def patch_new_price(cafe_id):
    new_price = request.args.get("new_price")
    cafe = db.session.query(Cafe).get(cafe_id)
    if cafe:
        cafe.coffee_price = new_price
        db.session.commit()
        # Just add the code after the jsonify method. 200 = Ok
        return jsonify(response={"success": "Successfully updated the price."}), 200
    else:
        # 404 = Resource not found
        return jsonify(error={"Not Found": "Sorry a cafe with that id was not found in the database."}), 404


## HTTP DELETE - Delete Record
@app.route("/report-closed/<int:cafe_id>", methods=["DELETE"])
def delete_cafe(cafe_id):
    api_key = request.args.get("api-key")
    if api_key == "TopSecretAPIKey":
        cafe = db.session.query(Cafe).get(cafe_id)
        if cafe:
            db.session.delete(cafe)
            db.session.commit()
            return jsonify(response={"success": "Successfully deleted the cafe from the database."}), 200
        else:
            return jsonify(error={"Not Found": "Sorry a cafe with that id was not found in the database."}), 404
    else:
        return jsonify(error={"Forbidden": "Sorry, that's not allowed. Make sure you have the correct api_key."}), 403


if __name__ == '__main__':
    app.run(debug=True)
