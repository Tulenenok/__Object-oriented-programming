#include "read_figure.h"


static err_t read_amount(int &amount, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    if (fscanf(file, "%d", &amount) != 1)
        return ERR_DATA;

    return OK;
}


static err_t read_point(dot_t &dot, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    if (fscanf(file, "%lf %lf %lf", &dot.x, &dot.y, &dot.z) != 3)
        return ERR_DATA;

    return OK;
}


static err_t read_all_dots(points_t &points, FILE *file)
{
    err_t rc = OK;

    for (int i = 0; i < points.len && rc == OK; i++) // in func (fixed)
        rc = read_point(points.data[i], file);

    return rc;
}


static err_t allocate_points(points_t &points)
{
    points.data = (dot_t*) malloc(points.len * sizeof(dot_t));

    if (points.data == NULL)
        return ERR_MEMORY_ALLOCATE;

    return OK;
}


static err_t read_points(points_t &points, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    int amount = 0;
    err_t rc = read_amount(amount, file);

    if (rc != OK)
        return rc;

    points.len = amount;

    rc = allocate_points(points);

    if (rc != OK)
        return rc;

    rc = read_all_dots(points, file);

    if (rc != OK)
        free_points(points);

    return rc;
}

static err_t allocate_connections_arr(links_t &links)
{
    links.data = (link_dots_t*) malloc(links.len * sizeof(link_dots_t));

    if (links.data == NULL)
        return ERR_MEMORY_ALLOCATE;

    return OK;
}


static err_t read_connection(link_dots_t &link, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    if (fscanf(file, "%d %d", &link.point_begin, &link.point_end) != 2)
        return ERR_DATA;

    return OK;
}


static err_t read_dots_connections(links_t &links, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    int amount = 0;
    err_t rc = read_amount(amount, file);

    if (rc != OK)
    {
        return rc;
    }

    links.len = amount;

    rc = allocate_connections_arr(links);

    if (rc != OK)
        return rc;

    for (int i = 0; i < amount && rc == OK; i++)
        rc = read_connection(links.data[i], file);

    if (rc != OK)
        free(links.data);

    return rc;
}


static err_t read_figure(figure_t &figure, FILE *file)
{
    if (file == NULL)
        return ERR_FILE_OPEN;

    figure = figure_init();

    err_t rc = read_points(figure.points, file);

    if (rc == OK)
    {
        rc = read_dots_connections(figure.links, file);

        if (rc != OK)
            free_points(figure.points);
    }

    return rc;
}

err_t load_figure(figure_t &figure, filename_t filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
        return ERR_FILE_OPEN;

    figure_t temp_figure;
    err_t rc = read_figure(temp_figure, file);
    fclose(file);

    if (rc == OK)
    {
        figure_free(figure);
        figure = temp_figure;
    }

    return rc;
}