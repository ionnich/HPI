#include "../include/dbstructs.h"
#include "../include/utils.h"
#include "stdlib.h"

void
initPatient(struct Patient *p) {
  strcpy(p->patientNumber, "");
  p->gender = ' ';
  p->age = -1;
  strcpy(p->name, "Nil");
}
void
getPatient(struct Patient *p) {
  ReferenceInput ref;

  String name;
  printf("What is your name?\n");
  getStr(name);
  strcpy(p->name, name);
  String patientNumber;
  printf("What is your patient number?\n");
  getStr(patientNumber);
  strcpy(p->patientNumber, patientNumber);
  printf("Enter your age:\n");
  inputHandler(INT, ref, &p->age);
  printf("Gender (M/F):\n");
  char Gender = ' ';
  strcpy(ref, "MF");
  inputHandler(CHAR, ref, (int *)&Gender);
  p->gender = Gender;
}

void
getPatientSymptoms(struct SymptomDB db, struct userSymps *us) {
  int i;
  char c;
  ReferenceInput ref = "YN";

  c = ' ';
  for (i = 0; i < db.count; i++) {
    printf("%s\n", db.database[i].question);
    inputHandler(CHAR, ref, (int *)&c);
    if (c == 'Y' || c == 'y') {
      us->arr[us->count] = db.database[i].id;
      strcpy(us->symName[us->count], db.database[i].name);
      us->count++;
    }
  }
}

int
validateImpression(struct Impression input, struct userSymps us) {

  int i;
  for (i = 0; i < input.sympCount; i++) {
    if (!isFound(us.arr, input.sympCount, input.correspondingSymptoms[i])) {
      return 0;
    }
  }
  return 1;
}

void
getPatientImpressions(struct SymptomDB sDB, struct ImpressionDB iDB,
                      struct userImps *ui, struct userSymps *us) {
  int i;
  //  initialize userSymps struct
  //  initialize userImps struct
  ui->count = 0;

  // get self report from user
  getPatientSymptoms(sDB, us);

  // loop through impressionDB
  for (i = 0; i < iDB.count; i++) {
    if (validateImpression(iDB.database[i], *us)) {
      ui->arr[ui->count] = iDB.database[i].id;
      strcpy(ui->impName[ui->count], iDB.database[i].name);
      ui->count++;
    }
  }
}

void
readPatientDetails(struct Patient p) {

  FILE *fp;
  char fileName[strlen(p.patientNumber) + 5];
  sprintf(fileName, "%s.txt", p.patientNumber);

  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("File not found\n");
    return;
  }

  char line[100];
  printf("\n");
  while (fgets(line, 100, fp) != NULL) {
    printf("%s", line);
  }
}

void
writeUserDetails(struct Patient p, struct userImps ui, struct userSymps us) {
  int i;
  FILE *fp;
  char fileName[strlen(p.patientNumber) + 5];
  sprintf(fileName, "%s.txt", p.patientNumber);
  fp = fopen(fileName, "w");
  fprintf(fp, "History of Present Illness\n\n");

  fprintf(fp, "%s, patient # ", p.name);
  fprintf(fp, "%s, is a ", p.patientNumber);
  fprintf(fp, "%d year old ", p.age);
  switch (p.gender) {
    case 'M':
    case 'm':
      fprintf(fp, "male. He has");
      break;
    case 'F':
    case 'f':
      fprintf(fp, "female. She has");
      break;
    default:
      break;
  }

  if (us.count == 0)
    fprintf(fp, "none! They are relatively healthy");
  else {
    for (i = 0; i < us.count - 1; i++) {
      fprintf(fp, " %s", us.symName[i]);

      if ((i - us.count - 1) >= 1)
        fprintf(fp, ",");
    }

    if (us.count > 1)
      fprintf(fp, " and");

    fprintf(fp, " %s.\n", us.symName[i]);
  }

  fprintf(fp, "Impressions are");

  if (ui.count == 0)
    fprintf(fp, "none! They are relatively healthy");
  else {
    for (i = 0; i < ui.count - 1; i++) {
      fprintf(fp, " %s", ui.impName[i]);

      if ((i - ui.count - 1) >= 1)
        fprintf(fp, ",");
    }

    if (ui.count > 1)
      fprintf(fp, " and");
    fprintf(fp, " %s.\n", ui.impName[i]);
  }

  fclose(fp);
}

void
PatientProcess(struct SymptomDB sDB, struct ImpressionDB iDB) {
  getchar(); // clear the trailing newline

  // initialize all necessary structs
  struct Patient p;
  struct userImps ui;
  struct userSymps us;
  initPatient(&p);
  us.count = 0;
  ui.count = 0;

  clear();

  // get patient details
  getPatient(&p);

  // get patient impressions
  getPatientImpressions(sDB, iDB, &ui, &us);

  // write patient details to file
  writeUserDetails(p, ui, us);

  readPatientDetails(p);

  printf("\n\nPatient details saved to %s.txt\n", p.patientNumber);

  getchar();
  sleepMs(500);
  con();
}