import requests
from bs4 import BeautifulSoup
import argparse

def get_real_field_type(field_type):
    types = {
        "Integer": "INTEGER",
        "True": "TRUE_FIELD",
        "Boolean": "BOOLEAN",
        "String": "STRING",
    }
    if field_type.startswith("Array of"):
        field_type = f"ARRAYOF, {get_real_field_type(field_type[8:])}"
    elif field_type in types.keys():
        field_type = types[field_type]
    else:
        field_type = f"COMPOUND, {field_type}"
    return field_type

def get_all_api(is_classes, is_methods):
    with open('TGAPI.html', 'r', encoding='utf-8') as file:
        html_content = file.read()
    soup = BeautifulSoup(html_content, 'html.parser')
    # Поиск всех заголовков <h4> и разделение их на классы и методы
    classes = []
    methods = []

    for tag in soup.find_all('h4'):
        text = tag.get_text(strip=True)
        if text and len(text.split()) == 1:
            if text[0].isupper():
                classes.append(text)
            elif text[0].islower():
                methods.append(text)
    if is_classes:
        for cls in classes:
            print(cls)

    if is_methods:
        for method in methods:
            print(method)

def load_api(outfile):
    url = "https://core.telegram.org/bots/api"
    
    response = requests.get(url)
    response.raise_for_status()
    with open(outfile, 'w', encoding='utf-8') as out:
        print(response.text, file=out)
    print(f"API stored into {outfile}")

def get_telegram_bot_api_fields(type_name, inapi="./TGAPI.html", outdir="."):

    with open(f'{inapi}', 'r', encoding='utf-8') as file:
        content = file.read()
    # Парсим HTML-страницу
    soup = BeautifulSoup(content, 'html.parser')
    
    # Ищем заголовок, соответствующий нужному типу
    type_header = soup.find('a', {'name': type_name.lower()})
    
    if not type_header:
        print(f"Type {type_name} not found on the page.")
        return
    
    # Ищем ближайшую таблицу после найденного заголовка
    table = type_header.find_next('table', {'class': 'table'})
    
    if not table:
        print(f"No table found for type {type_name}")
        return
    
    # Извлекаем строки таблицы
    rows = table.find_all('tr')[1:]  # Пропускаем заголовок таблицы
    
    with open(f'{outdir}/{type_name}.def', 'w', encoding='utf-8') as file:
        file.write("#include \"tgbm/api_types/def/generators/begin_file.h\"\n\n")
        file.write(f"API_TYPE({type_name})\n\n")
        for row in rows:
            columns = row.find_all('td')
            if len(columns) >= 3:
                field_name = columns[0].get_text(strip=True)
                field_type = columns[1].get_text(strip=True)
                field_description = columns[2].get_text(strip=True)
                
                if field_description.startswith('Optional'):
                    file.write(f"// {field_description[10:]}\n")
                    file.write("OPTIONAL(")
                else:
                    file.write(f"// {field_description}\n")
                    file.write("REQUIRED(")
                file.write(f"{get_real_field_type(field_type)}, {field_name})\n\n")
        file.write("#include \"tgbm/api_types/def/generators/end_file.h\"\n")
    print(f"Data for type {type_name} has been saved to {outdir}/{type_name}.def")

def main():
    parser = argparse.ArgumentParser(description='Extract fields and descriptions for a given type from the Telegram Bot API.')
    parser.add_argument('--types', nargs='+', type=str, help='The name of the type to extract fields for (e.g., Message, Update).')
    parser.add_argument('--apipath', type=str, help="api.html file path", default="./TGAPI.html")
    parser.add_argument('--outdir', type=str, help="output for classes .def files", default='.')
    parser.add_argument('--get_methods', action='store_true', help="gets all api methods and exits")
    parser.add_argument('--get_classes', action='store_true', help="gets all api classes and exits")
    parser.add_argument('--load_api', type=str, help="loads TG API to file, usage: --load_api=<path>")
    args = parser.parse_args()
    if args.get_methods or args.get_classes:
        get_all_api(args.get_classes, args.get_methods)
        return
    if args.load_api:
        load_api(args.load_api)
        return
    if args.types:
        for t in args.types:
            get_telegram_bot_api_fields(t, args.apipath, args.outdir)
        return
    parser.print_usage()

if __name__ == '__main__':
    main()
