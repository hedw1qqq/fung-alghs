#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "string.h"

typedef struct Address {
    String city;
    String street;
    unsigned int house_number;
    String building;
    unsigned int apartment_number;
    String postal_code;
} Address;

typedef struct Mail {
    Address recipient;
    double weight;
    String postal_id;
    String creation_time;
    String delivery_time;
} Mail;

typedef struct Post {
    Address *office_address;
    Mail *mails;
    size_t mail_count;
    size_t capacity;
} Post;

int is_numeric(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

int parse_time(const String *str, struct tm *time) {

    memset(time, 0, sizeof(struct tm));
    char *result = strptime(str->val, "%d:%m:%Y %H:%M:%S", time);
    if (result == NULL) {
        printf("Error parsing date: %s\n", str->val);
        return 0;
    }

    return 1;
}

int validate_address(const Address *address) {
    if (address->city.length == 0 || address->street.length == 0) {
        printf("City and Street cannot be empty.\n");
        return 0;
    }
    if (address->house_number <= 0) {
        printf("House number must be a positive integer and reasonable value.\n");
        return 0;
    }
    if (address->apartment_number <= 0 || address->apartment_number > 1000) {
        printf("Apartment number must be a positive integer and reasonable value.\n");
        return 0;
    }
    if (address->postal_code.length != 6 || !is_numeric(address->postal_code.val)) {
        printf("Postal code must be exactly 6 digits.\n");
        return 0;
    }
    return 1;
}

int validate_mail(const Mail *mail) {
    if (mail->weight <= 0) {
        printf("Weight must be a positive number.\n");
        return 0;
    }
    if (mail->postal_id.length != 14 || !is_numeric(mail->postal_id.val)) {
        printf("Postal ID must be exactly 14 digits.\n");
        return 0;
    }

    struct tm creation_tm, delivery_tm;
    if (!parse_time(&mail->creation_time, &creation_tm) || !parse_time(&mail->delivery_time, &delivery_tm)) {
        printf("Creation and delivery times must be in the format dd:MM:yyyy hh:mm:ss.\n");
        return 0;
    }

    time_t creation_time = mktime(&creation_tm);
    time_t delivery_time = mktime(&delivery_tm);

    if (creation_time == -1 || delivery_time == -1) {
        printf("Failed to convert times to timestamp. Check date and time values.\n");
        return 0;
    }

    if (difftime(creation_time, delivery_time) > 0) {
        printf("Creation time cannot be later than delivery time.\n");
        return 0;
    }

    return 1;
}

void print_mail(const Mail *mail) {
    printf("Postal ID: %s\n", mail->postal_id.val);
    printf("Recipient City: %s\n", mail->recipient.city.val);
    printf("Recipient Street: %s\n", mail->recipient.street.val);
    printf("Recipient House: %u\n", mail->recipient.house_number);
    printf("Building: %s\n", mail->recipient.building.val);
    printf("Apartment: %u\n", mail->recipient.apartment_number);
    printf("Postal Code: %s\n", mail->recipient.postal_code.val);
    printf("Weight: %.2f\n", mail->weight);
    printf("Creation Time: %s\n", mail->creation_time.val);
    printf("Delivery Time: %s\n\n", mail->delivery_time.val);
}

int compare_mail(const void *a, const void *b) {
    Mail *mail1 = (Mail *) a;
    Mail *mail2 = (Mail *) b;
    int cmp = compare_str(mail1->recipient.postal_code, mail2->recipient.postal_code);
    if (cmp == 0) {
        cmp = compare_str(mail1->postal_id, mail2->postal_id);
    }
    return cmp;
}

void sort_mails(Post *post) {
    qsort(post->mails, post->mail_count, sizeof(Mail), compare_mail);
}

void destroy_mail(Mail *mail) {
    destroy_str(&mail->recipient.city);
    destroy_str(&mail->recipient.street);
    destroy_str(&mail->recipient.building);
    destroy_str(&mail->recipient.postal_code);
    destroy_str(&mail->postal_id);
    destroy_str(&mail->creation_time);
    destroy_str(&mail->delivery_time);
}

void add_mail(Post *post, Mail new_mail) {
    if (!validate_address(&new_mail.recipient) || !validate_mail(&new_mail)) {
        printf("Failed to add mail due to invalid input.\n");
        destroy_mail(&new_mail);

        return;
    }

    if (post->mail_count >= post->capacity) {
        size_t new_capacity = post->capacity ? post->capacity * 2 : 1;
        Mail *new_mails = realloc(post->mails, new_capacity * sizeof(Mail));
        if (!new_mails) {
            printf("Memory allocation failed.\n");
            destroy_mail(&new_mail);

            return;
        }

        post->mails = new_mails;
        post->capacity = new_capacity;
    }

    post->mails[post->mail_count++] = new_mail;
    sort_mails(post);
}


void remove_mail(Post *post, char *postal_id) {
    String postal_id_str;
    create_str(&postal_id_str, postal_id);
    for (size_t i = 0; i < post->mail_count; ++i) {
        if (equiv_str(post->mails[i].postal_id, postal_id_str)) {
            destroy_mail(&post->mails[i]);

            for (size_t j = i; j < post->mail_count - 1; ++j) {
                post->mails[j] = post->mails[j + 1];
            }

            post->mail_count--;
            printf("Mail removed successfully.\n");
            destroy_str(&postal_id_str);
            sort_mails(post);
            return;
        }
    }
    printf("Mail with ID %s not found.\n", postal_id);
    destroy_str(&postal_id_str);
}

Mail *find_mail_by_id(const Post *post, char *postal_id) {
    String postal_id_str;
    create_str(&postal_id_str, postal_id);
    for (size_t i = 0; i < post->mail_count; ++i) {
        if (equiv_str(post->mails[i].postal_id, postal_id_str)) {
            destroy_str(&postal_id_str);
            return &post->mails[i];
        }
    }
    destroy_str(&postal_id_str);
    return NULL;
}


int is_delivered(const Mail *mail) {
    struct tm delivery_tm;
    time_t current_time = time(NULL);

    if (parse_time(&mail->delivery_time, &delivery_tm)) {
        time_t delivery_time = mktime(&delivery_tm);
        return difftime(current_time, delivery_time) >= 0;
    }
    return 0;
}

int is_expired(const Mail *mail) {
    struct tm delivery_tm;
    time_t current_time = time(NULL);

    if (parse_time(&mail->delivery_time, &delivery_tm)) {
        time_t delivery_time = mktime(&delivery_tm);
        return difftime(current_time, delivery_time) < 0;
    }
    return 0;
}

void list_all_mails(const Post *post) {
    for (size_t i = 0; i < post->mail_count; ++i) {
        print_mail(&post->mails[i]);
    }
}

void list_delivered_mails(const Post *post) {

    Mail *delivered_mails = malloc(post->mail_count * sizeof(Mail));
    size_t delivered_count = 0;


    for (size_t i = 0; i < post->mail_count; ++i) {
        if (is_delivered(&post->mails[i])) {
            delivered_mails[delivered_count++] = post->mails[i];
        }
    }

    for (size_t i = 0; i < delivered_count; ++i) {
        print_mail(&delivered_mails[i]);
    }

    free(delivered_mails);
}

void list_expired_mails(const Post *post) {

    Mail *expired_mails = malloc(post->mail_count * sizeof(Mail));
    size_t expired_count = 0;

    for (size_t i = 0; i < post->mail_count; ++i) {
        if (is_expired(&post->mails[i])) {
            expired_mails[expired_count++] = post->mails[i];
        }
    }

    for (size_t i = 0; i < expired_count; ++i) {
        print_mail(&expired_mails[i]);
    }

    free(expired_mails);
}

void destroy_post(Post *post) {

    for (size_t i = 0; i < post->mail_count; ++i) {

        destroy_mail(&post->mails[i]);
    }

    free(post->mails);
    post->mails = NULL;
    post->mail_count = 0;
    post->capacity = 0;

    if (post->office_address != NULL) {

        destroy_str(&post->office_address->city);
        destroy_str(&post->office_address->street);
        destroy_str(&post->office_address->building);
        destroy_str(&post->office_address->postal_code);

        free(post->office_address);
        post->office_address = NULL;
    }
}

int main() {
    Post post_office = {0};
    post_office.mails = NULL;
    post_office.mail_count = 0;
    post_office.capacity = 0;


    int choice;
    while (1) {
        printf("1. Add Mail\n2. Remove Mail\n3. Find Mail\n4. List Delivered Mails\n5. List Expired Mails\n6. List All Mails\n7. Exit\n");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice == 1) {
            Mail mail;
            char buffer[100];
            int valid = 1;

            printf("Enter City: ");
            scanf("%99s",buffer);
            if (create_str(&mail.recipient.city, buffer) != ok) valid = 0;

            printf("Enter Street: ");
            scanf("%99s",buffer);
            if (valid && create_str(&mail.recipient.street, buffer) != ok) valid = 0;
            printf("Enter house number: ");
            scanf("%d", &mail.recipient.house_number);
            getchar();

            printf("Enter Building: ");
            scanf("%99s",buffer);
            if (valid && create_str(&mail.recipient.building, buffer) != ok) valid = 0;
            printf("Enter apartment number: ");
            scanf("%d", &mail.recipient.apartment_number);
            getchar();

            printf("Enter Postal Code (6 digits): ");
            scanf("%99s",buffer);
            if (valid && create_str(&mail.recipient.postal_code, buffer) != ok) valid = 0;

            printf("Enter Postal ID (14 digits): ");
            scanf("%99s",buffer);
            if (valid && create_str(&mail.postal_id, buffer) != ok) valid = 0;
            printf("Enter weight: ");
            scanf("%lf", &mail.weight);
            getchar();
            printf("Enter Creation Time (dd:MM:yyyy hh:mm:ss): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (valid && create_str(&mail.creation_time, buffer) != ok) valid = 0;

            printf("Enter Delivery Time (dd:MM:yyyy hh:mm:ss): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (valid && create_str(&mail.delivery_time, buffer) != ok) valid = 0;

            if (valid) {
                add_mail(&post_office, mail);
            } else {
                destroy_mail(&mail);
                printf("Failed to create mail due to memory allocation error.\n");
            }

        } else if (choice == 2) {
            char buffer[100];
            printf("Enter Postal ID to remove: ");
            scanf("%s", buffer);
            remove_mail(&post_office, buffer);

        } else if (choice == 3) {
            char buffer[100];
            printf("Enter Postal ID to find: ");
            scanf("%s", buffer);
            Mail *mail = find_mail_by_id(&post_office, buffer);
            if (mail) print_mail(mail);
            else printf("Mail not found.\n");

        } else if (choice == 4) {
            list_delivered_mails(&post_office);

        } else if (choice == 5) {
            list_expired_mails(&post_office);

        } else if (choice == 6) {
            list_all_mails(&post_office);

        } else if (choice == 7) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    destroy_post(&post_office);
    return 0;
}
