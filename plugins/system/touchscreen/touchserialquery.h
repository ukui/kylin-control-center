#ifndef TOUCHSERIALQUERY_H
#define TOUCHSERIALQUERY_H

int find_touchId_from_event(int pId ,char *_event);
int find_event_from_name(char *_name, char *_event, char *_serial);
int findSerialFromId(int touchid,char *touchname,char *_touchserial,int maxlen);

#endif // TOUCHSERIALQUERY_H
