**Testrapport**

**Projectnaam:** S.P.I.T.S. 2  
**Tester:** Alexander  
**Datum:** 26 juni 2024  
**Doel van de Test:** Verifiëren van de beveiliging van de invoerparameters tegen SQL-injectie kwetsbaarheden en de correcte verwerking van data door `inserttestdata.php`.

**Beschrijving van de Testomgeving:**
- **Server URL:** http://192.168.50.188/inserttestdata.php
- **Client:** Windows 10
- **Tool gebruikt:** SQLMap v1.8.6.11#dev
- **Python Versie:** Python 3.9

**Test Procedure:**
1. **Installatie van SQLMap:**
   - SQLMap is geïnstalleerd via Python's pip pakketbeheerder:
     ```sh
     pip install sqlmap
     ```

2. **Uitvoering van SQLMap:**
   - De volgende opdracht is uitgevoerd om te testen op SQL-injectie kwetsbaarheden in de POST-parameters van `inserttestdata.php`:
     ```sh
     python sqlmap.py -u "http://192.168.50.188/inserttestdata.php" --data="imei=1&iccid=1&firmware=1" --batch --dump
     ```

3. **Samenvatting van de Resultaten:**
   - **Verbinding met doel-URL:** Succesvol
   - **Stabiliteit van de URL-inhoud:** Stabiel
   - **Testen van dynamiek van POST parameters:** POST parameters ('imei', 'iccid', 'firmware') bleken niet dynamisch.
   - **Heuristische tests:** POST parameters ('imei', 'iccid', 'firmware') leken niet injecteerbaar.
   - **Tests voor SQL-injectie technieken:** Verschillende technieken zoals AND boolean-based blind, error-based, inline queries, stacked queries, time-based blind, en UNION queries zijn getest op de POST parameters ('imei', 'iccid', 'firmware').
   - **Resultaat:** Geen injecteerbare parameters gevonden.

**Conclusie:**
- Geen SQL-injectie kwetsbaarheden gevonden in de geteste parameters ('imei', 'iccid', 'firmware').
- De beveiligingsmaatregelen, zoals het gebruik van prepared statements en input validatie, blijken effectief te zijn.