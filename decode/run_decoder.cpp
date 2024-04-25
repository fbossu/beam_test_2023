#inclue "Decoder.cpp"

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    Decoder decoder(argv[1]);
    decoder.Decode();

    return 0;
}
