import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Query } from './query.model';
import { Observable } from 'rxjs';
import { map } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class QueryService {

  constructor(private readonly http: HttpClient) { }

  public query(query: Query): Observable<string[]> {
    return this.http.post<any>('http://localhost:8080/query', query).pipe(
      map(body => body.results)
    );
  }
}
