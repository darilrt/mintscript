import json
import zs.http

mut server := new http.Server()

server.route("/api/v1/users", "GET", (req : http.Request, res : http.Response) {
    mut json : json.Object = {
        "users": [
            {
                "id": 1,
                "name": "John Doe",
                "email": "JohnDoe@example.com",
            },
            {
                "id": 2,
                "name": "Jane Doe",
                "email": "JaneDoe@example.com",
            },
        ]
    }

    res.send(json)
}
)

server.listen(8080)
