class User {
  User(this.tag, this.ime, this.prezime, this.isPresent);

  final String tag;
  final String ime;
  final String prezime;
  bool isPresent;

  @override
  String toString() {
    return "$ime $prezime";
  }
}
