import argparse
import requests
import typing

# loads TG api (html)
def load_api_html() -> str:
    url = "https://core.telegram.org/bots/api"
    response = requests.get(url)
    if response.status_code != 200:
        raise ValueError(f"[TGBM][ERROR]: cannod load api, status: {response.status_code}")
    return response.text

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", type=str, required=True, help="output file for TG API(without quotes)")

    args = parser.parse_args()

    with open(args.out, 'w', encoding='utf-8') as file:
        file.write(load_api_html())
    print(f'[TGBM] api loaded into {args.out}')

if __name__ == '__main__':
    main()
