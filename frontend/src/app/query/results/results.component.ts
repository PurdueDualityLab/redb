import { Component, Input, OnInit } from '@angular/core';
import { Clipboard } from '@angular/cdk/clipboard';
import { MatSnackBar } from '@angular/material/snack-bar';

@Component({
  selector: 'app-results',
  templateUrl: './results.component.html',
  styleUrls: ['./results.component.scss']
})
export class ResultsComponent implements OnInit {

  @Input()
  results?: string[] | null = [];

  @Input()
  isLoading?: boolean | null = false;

  constructor(
    private readonly clipboard: Clipboard,
    private readonly matSnackBar: MatSnackBar
  ) { }

  ngOnInit(): void {
  }

  setRegexToClipboard(content: string) {
    const isSuccessful = this.clipboard.copy(content);
    if (isSuccessful) {
      this.matSnackBar.open(`Copied regex ${content} to clipboard`, "CLOSE", { duration: 5000 });
    } else {
      this.matSnackBar.open(`Failed to copy regex to clipboard`, "CLOSE", { duration: 5000 });
    }
  }
}
