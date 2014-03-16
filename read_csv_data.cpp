using namespace std;

struct csv_state {
  string name;
  int correctcol;
  int currentcol;
  int row;
  double* data;
};



void cb3 (void *s, size_t len, void *data) {
  struct csv_state* temp = (csv_state*) data;
  if(temp->correctcol == -1) {
    if (string((const char*) s, len) == temp->name) {
      temp->correctcol = temp->currentcol;
    }
  } else if (temp->correctcol == temp->currentcol) {
    temp->data[temp->row] = atof(string((const char*) s, len).c_str());
  }
  temp->currentcol++;
}

void cb4 (int c, void *data) {
  struct csv_state* temp = (csv_state*) data;
  temp->row++;
  temp->currentcol = 0;
}



struct data_array
read_csv_data(char* filename, char* type)
{
  FILE *fp;
  struct csv_parser p;
  struct data_array out;
  char buf[1024];
  size_t bytes_read;
  unsigned char options = 0;
  struct csv_state temp = {string(type), -1, 0, -1, NULL};

  if (csv_init(&p, options) != 0) {
    fprintf(stderr, "Failed to initialize csv parser\n");
    exit(EXIT_FAILURE);
  }

  csv_set_space_func(&p, is_space);
  csv_set_term_func(&p, is_term);

    fp = fopen(filename, "rb");
    if (!fp) {
      fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
      exit(EXIT_FAILURE);
    }

    temp.data = new double[count_lines(fp)-1];

    rewind(fp);

    while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
      if (csv_parse(&p, buf, bytes_read, cb3, cb4, &temp) != bytes_read) {
        fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(&p)));
      }
    }

    csv_fini(&p, cb3, NULL, &temp);

    if (ferror(fp)) {
      fprintf(stderr, "Error while reading file %s\n", filename);
      fclose(fp);
      exit(EXIT_FAILURE);
    }

    fclose(fp);

  csv_free(&p);

  out.len = temp.row;
  out.array = temp.data;

  return out;
}
