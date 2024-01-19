import module_name.*
import module_name.x
import module_name { x, y }
import * from "file.mint"
import { x } from "file.mint"

HomeController:: class(Controller) {
    user_repository: UsersRepository

    [GET('/get_user/<user_id>')]
    index:: fn(user_id: int) -> Response {
        user: User? = user_repository.get(user_id);

        if (user == null) {
            return Response.make('User not found', 404);
        }

        return Response.make(user.to_json());
    }
}

UsersRepository:: trait {
    get:: (id: int) -> User
}

UsersRepositoryImpl:: class(UsersRepository) {
    get:: (id: int) -> User {
        return User.find(id);
    }
}

User:: class {
    id: int
    name: string
    email: string

    to_dict:: () -> Json {
        return {
            'id': this.id,
            'name': this.name,
            'email': this.email
        };
    }
}
